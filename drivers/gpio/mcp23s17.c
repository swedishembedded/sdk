// SPDX-License-Identifier: Apache-2.0
/**
 * Copyright 2020 Geanix ApS
 * Copyright 2022 Martin Schröder <info@swedishembedded.com>
 **/

#define DT_DRV_COMPAT microchip_mcp23s17v2

#include <device.h>
#include <kernel.h>
#include <logging/log.h>
#include <drivers/gpio.h>
#include <drivers/spi.h>
#include <drivers/gpio/gpio_utils.h>

#include <sys/byteorder.h>
#include <sys/util.h>

/** Register log module */
LOG_MODULE_REGISTER(mcp23s17);

#define REG_IODIRA 0x00
#define REG_IODIRB 0x01
#define REG_IPOLA 0x02
#define REG_IPOLB 0x03
#define REG_GPINTENA 0x04
#define REG_GPINTENB 0x05
#define REG_DEFVALA 0x06
#define REG_DEFVALB 0x07
#define REG_INTCONA 0x08
#define REG_INTCONB 0x09
#define REG_IOCONA 0x0A
#define REG_IOCONB 0x0B
#define REG_GPPUA 0x0C
#define REG_GPPUB 0x0D
#define REG_INTFA 0x0E
#define REG_INTFB 0x0F
#define REG_INTCAPA 0x10
#define REG_INTCAPB 0x11
#define REG_PORTA 0x12
#define REG_PORTB 0x13
#define REG_OLATA 0x14
#define REG_OLATB 0x15

/** Driver private data */
struct mcp23s17 {
	/** Driver data needs to be first */
	struct gpio_driver_data data;
	/** Device pointer (used for gpio callbacks) */
	const struct device *dev;
	/** Device lock */
	struct k_mutex lock;
	/** Callback list */
	sys_slist_t cb;
	/** Interrupt A callback */
	struct gpio_callback inta_cb;
	/** Interrupt B callback */
	struct gpio_callback intb_cb;
	/** Work for handling IRQs */
	struct k_work irq_work;
	/** Register cache */
	struct {
		/** I/O direction register A*/
		uint8_t IODIRA;
		/** I/O direction register B */
		uint8_t IODIRB;
		/** I/O polarity register A */
		uint8_t IPOLA;
		/** I/O polarity register B */
		uint8_t IPOLB;
		/** Interrupt on change event enable register A */
		uint8_t GPINTENA;
		/** Interrupt on change event enable register B */
		uint8_t GPINTENB;
		/** Default value register A */
		uint8_t DEFVALA;
		/** Default value register B */
		uint8_t DEFVALB;
		/** Interrupt on change control register A */
		uint8_t INTCONA;
		/** Interrupt on change control register B */
		uint8_t INTCONB;
		/** Configuration register A */
		uint8_t IOCONA;
		/** Configuration register B */
		uint8_t IOCONB;
		/** GPIO pull up resistor register A */
		uint8_t GPPUA;
		/** GPIO pull up resistor register B */
		uint8_t GPPUB;
		/** Interrupt flag register A */
		uint8_t INTFA;
		/** Interrupt flag register B */
		uint8_t INTFB;
		/** Interrupt capture register A */
		uint8_t INTCAPA;
		/** Interrupt capture register B */
		uint8_t INTCAPB;
		/** Data register A */
		uint8_t PORTA;
		/** Data register B */
		uint8_t PORTB;
		/** Output latch register A */
		uint8_t OLATA;
		/** Output latch register B */
		uint8_t OLATB;
	} __packed regs;
};

/** Driver configuration */
struct mcp23s17_config {
	/** Device address */
	uint8_t addr;
	/** SPI bus */
	struct spi_dt_spec spi;
	/** Interrupts */
	struct gpio_dt_spec inta_gpio, intb_gpio;
};

/**
 * @brief Read both port 0 and port 1 registers of certain register function.
 *
 * Given the register in reg, read the pair of port 0 and port 1.
 *
 * @param dev Device struct of the MCP23S17.
 * @param reg Register to read (the PORTA of the pair of registers).
 * @param buf Buffer to read data into.
 *
 * @return 0 if successful, failed otherwise.
 */
static int _mcp23s17_read_regs(const struct device *dev, uint8_t reg, uint8_t *buf, size_t size)
{
	if (!dev || !buf) {
		return -EINVAL;
	}

	const struct mcp23s17_config *config = dev->config;
	int ret;
	uint8_t addr = config->addr | 1;
	uint8_t buffer_tx[] = { 0x40 | addr, reg };

	const struct spi_buf tx_buf[] = {
		{
			.buf = buffer_tx,
			.len = sizeof(buffer_tx),
		},
	};
	const struct spi_buf_set tx = {
		.buffers = tx_buf,
		.count = ARRAY_SIZE(tx_buf),
	};
	const struct spi_buf rx_buf[2] = {
		{
			.buf = NULL,
			.len = sizeof(buffer_tx),
		},
		{
			.buf = (uint8_t *)buf,
			.len = size,
		},
	};
	const struct spi_buf_set rx = {
		.buffers = rx_buf,
		.count = ARRAY_SIZE(rx_buf),
	};

	ret = spi_transceive_dt(&config->spi, &tx, &rx);
	if (ret) {
		LOG_DBG("spi_transceive FAIL %d\n", ret);
		return -EIO;
	}

	return 0;
}

/**
 * @brief Write both port 0 and port 1 registers of certain register function.
 *
 * Given the register in reg, write the pair of port 0 and port 1.
 *
 * @param dev Device struct of the MCP23S17.
 * @param reg Register to write into (the PORTA of the pair of registers).
 * @param buf Buffer to write data from.
 * @param size Size of data to write (will be written sequentially)
 *
 * @return 0 if successful, failed otherwise.
 */
static int _mcp23s17_write_regs(const struct device *dev, uint8_t reg, uint8_t *buf, size_t size)
{
	const struct mcp23s17_config *config = dev->config;

	uint8_t buffer_tx[2] = { 0x40 | config->addr, reg };

	const struct spi_buf tx_buf[2] = {
		{
			.buf = buffer_tx,
			.len = 2,
		},
		{
			.buf = buf,
			.len = (uint8_t)size,
		},
	};

	const struct spi_buf_set tx = {
		.buffers = tx_buf,
		.count = ARRAY_SIZE(tx_buf),
	};

	int ret = spi_write_dt(&config->spi, &tx);

	if (ret != 0) {
		LOG_ERR("spi_write FAIL: %d\n", ret);
		return -EIO;
	}

	return 0;
}

/**
 * @brief Setup the pin direction (input or output)
 *
 * @param dev Device struct of the MCP23S17
 * @param pin The pin number
 * @param flags Flags of pin or port
 *
 * @return 0 if successful, failed otherwise
 */
static int _mcp23s17_pin_configure_direction(const struct device *dev, uint32_t pin, int flags)
{
	struct mcp23s17 *self = (struct mcp23s17 *)dev->data;

	uint16_t dir = ((uint16_t)self->regs.IODIRB << 8) | self->regs.IODIRA;
	uint16_t output = ((uint16_t)self->regs.PORTB << 8) | self->regs.PORTA;

	if ((flags & GPIO_OUTPUT) != 0) {
		if ((flags & GPIO_OUTPUT_INIT_HIGH) != 0) {
			output |= BIT(pin);
		} else if ((flags & GPIO_OUTPUT_INIT_LOW) != 0) {
			output &= ~BIT(pin);
		}
		dir &= ~BIT(pin);
	} else {
		dir |= BIT(pin);
	}

	{
		uint8_t buf[2] = { dir, dir >> 8 };

		if (_mcp23s17_write_regs(dev, REG_IODIRA, buf, sizeof(buf)) != 0) {
			LOG_ERR("Error writing registers");
			return -EIO;
		}
		self->regs.IODIRA = buf[0];
		self->regs.IODIRB = buf[1];
	}

	{
		uint8_t buf[2] = { output, output >> 8 };

		if (_mcp23s17_write_regs(dev, REG_PORTA, buf, sizeof(buf)) != 0) {
			LOG_ERR("Error writing registers");
			return -EIO;
		}
		self->regs.PORTA = buf[0];
		self->regs.PORTB = buf[1];
	}

	return 0;
}

/**
 * @brief Setup the pin pull up/pull down status
 *
 * @param dev Device struct of the MCP23S17
 * @param pin The pin number
 * @param flags Flags of pin or port
 *
 * @return 0 if successful, failed otherwise
 */
static int _mcp23s17_pin_configure_pullup(const struct device *dev, uint32_t pin, int flags)
{
	struct mcp23s17 *self = (struct mcp23s17 *)dev->data;

	uint16_t gppu = ((uint16_t)self->regs.GPPUB << 8) | self->regs.GPPUA;

	/* pull down == 0, pull up == 1 */
	if ((flags & GPIO_PULL_DOWN) != 0U) {
		return -ENOTSUP;
	}

	WRITE_BIT(gppu, pin, (flags & GPIO_PULL_UP) != 0U);

	uint8_t buf[] = { gppu, gppu >> 8 };

	if (_mcp23s17_write_regs(dev, REG_GPPUA, buf, sizeof(buf)) != 0) {
		return -EIO;
	}
	self->regs.GPPUA = buf[0];
	self->regs.GPPUB = buf[1];

	return 0;
}

static int _mcp23s17_pin_configure(const struct device *dev, gpio_pin_t pin, gpio_flags_t flags)
{
	int ret = 0;
	struct mcp23s17 *self = dev->data;

	/* Can't do SPI bus operations from an ISR */
	if (k_is_in_isr()) {
		return -EWOULDBLOCK;
	}

	k_mutex_lock(&self->lock, K_FOREVER);

	if ((flags & GPIO_OPEN_DRAIN) == GPIO_OPEN_DRAIN) {
		ret = -ENOTSUP;
		goto done;
	}

	if (_mcp23s17_pin_configure_direction(dev, pin, flags) != 0) {
		LOG_ERR("MCP23S17: error setting pin direction (%d)", ret);
		ret = -EIO;
		goto done;
	}

	if (_mcp23s17_pin_configure_pullup(dev, pin, flags) != 0) {
		LOG_ERR("MCP23S17: error setting pin pull up/down (%d)", ret);
		ret = -EIO;
		goto done;
	}

done:
	k_mutex_unlock(&self->lock);
	return ret;
}

static int _mcp23s17_port_get_raw(const struct device *dev, uint32_t *value)
{
	if (!value) {
		return -EINVAL;
	}

	struct mcp23s17 *self = dev->data;
	uint8_t buf[2];
	int ret = 0;

	/* Can't do SPI bus operations from an ISR */
	if (k_is_in_isr()) {
		return -EWOULDBLOCK;
	}

	k_mutex_lock(&self->lock, K_FOREVER);

	if (_mcp23s17_read_regs(dev, REG_PORTA, buf, sizeof(buf)) != 0) {
		LOG_ERR("Failed to read port registers");
		ret = -EIO;
		goto done;
	}

	*value = ((uint32_t)buf[1] << 8) | buf[0];

	self->regs.PORTA = buf[0];
	self->regs.PORTB = buf[1];
done:
	k_mutex_unlock(&self->lock);
	return ret;
}

static int _mcp23s17_port_set_masked_raw(const struct device *dev, uint32_t mask, uint32_t value)
{
	struct mcp23s17 *self = dev->data;
	uint16_t gpio = ((uint16_t)self->regs.PORTB << 8) | self->regs.PORTA;

	/* Can't do SPI bus operations from an ISR */
	if (k_is_in_isr()) {
		return -EWOULDBLOCK;
	}

	k_mutex_lock(&self->lock, K_FOREVER);

	gpio = (gpio & ~mask) | (mask & value);

	uint8_t buf[] = { gpio, gpio >> 8 };

	if (_mcp23s17_write_regs(dev, REG_PORTA, buf, sizeof(buf)) != 0) {
		return -EIO;
	}

	self->regs.PORTA = buf[0];
	self->regs.PORTB = buf[1];

	k_mutex_unlock(&self->lock);

	return 0;
}

static int _mcp23s17_port_set_bits_raw(const struct device *dev, uint32_t mask)
{
	return _mcp23s17_port_set_masked_raw(dev, mask, mask);
}

static int _mcp23s17_port_clear_bits_raw(const struct device *dev, uint32_t mask)
{
	return _mcp23s17_port_set_masked_raw(dev, mask, 0);
}

static int _mcp23s17_port_toggle_bits(const struct device *dev, uint32_t mask)
{
	struct mcp23s17 *const self = (struct mcp23s17 *const)dev->data;
	uint16_t gpio = ((uint16_t)self->regs.PORTB << 8) | self->regs.PORTA;
	int ret = 0;

	/* Can't do SPI bus operations from an ISR */
	if (k_is_in_isr()) {
		return -EWOULDBLOCK;
	}

	k_mutex_lock(&self->lock, K_FOREVER);

	gpio ^= mask;

	uint8_t buf[] = { gpio, gpio >> 8 };

	if (_mcp23s17_write_regs(dev, REG_PORTA, buf, sizeof(buf)) != 0) {
		ret = -EIO;
		goto error;
	}

	self->regs.PORTA = buf[0];
	self->regs.PORTB = buf[1];
error:
	k_mutex_unlock(&self->lock);
	return ret;
}

static int _mcp23s17_pin_interrupt_configure(const struct device *dev, gpio_pin_t pin,
					     enum gpio_int_mode mode, enum gpio_int_trig trig)
{
	struct mcp23s17 *self = (struct mcp23s17 *)dev->data;
	struct mcp23s17_config *config = (struct mcp23s17_config *)dev->config;
	int ret = 0;

	if (pin < 8 && config->inta_gpio.port == NULL) {
		return -ENOTSUP;
	}

	if (pin >= 8 && config->intb_gpio.port == NULL) {
		return -ENOTSUP;
	}

	k_mutex_lock(&self->lock, K_FOREVER);

	uint16_t GPINTEN = ((uint16_t)self->regs.GPINTENB << 8) | self->regs.GPINTENA;
	uint16_t DEFVAL = ((uint16_t)self->regs.DEFVALB << 8) | self->regs.DEFVALA;
	/** 1: defval, 0: gpio */
	uint16_t INTCON = ((uint16_t)self->regs.INTCONB << 8) | self->regs.INTCONA;

	switch (mode) {
	case GPIO_INT_MODE_DISABLED:
		GPINTEN &= ~BIT(pin);
		break;
	case GPIO_INT_MODE_EDGE: {
		GPINTEN |= BIT(pin);
		if (trig == GPIO_INT_TRIG_LOW) {
			// compare against defval
			INTCON |= BIT(pin);
			// set defval to 1
			DEFVAL |= BIT(pin);
		} else if (trig == GPIO_INT_TRIG_HIGH) {
			INTCON |= BIT(pin);
			// set defval to 0
			DEFVAL &= ~BIT(pin);
		} else {
			INTCON &= ~BIT(pin);
		}
	} break;
	default:
		k_mutex_unlock(&self->lock);
		return -ENOTSUP;
	}

	// write registers
	uint8_t buf[] = {
		GPINTEN, GPINTEN >> 8, DEFVAL, DEFVAL >> 8, INTCON, INTCON >> 8,
	};

	if (_mcp23s17_write_regs(dev, REG_GPINTENA, buf, sizeof(buf)) != 0) {
		ret = -EIO;
		goto error;
	}

	self->regs.GPINTENA = buf[0];
	self->regs.GPINTENB = buf[1];
	self->regs.DEFVALA = buf[2];
	self->regs.DEFVALB = buf[3];
	self->regs.INTCONA = buf[4];
	self->regs.INTCONB = buf[5];

error:
	k_mutex_unlock(&self->lock);
	return ret;
}

static int _mcp23s17_manage_callback(const struct device *dev, struct gpio_callback *callback,
				     bool set)
{
	struct mcp23s17 *self = (struct mcp23s17 *)dev->data;

	return gpio_manage_callback(&self->cb, callback, set);
}

static uint32_t _mcp23s17_get_pending_int(const struct device *dev)
{
	struct mcp23s17 *self = dev->data;
	uint8_t buf[2];
	uint32_t ret = 0;

	/* Can't do SPI bus operations from an ISR */
	if (k_is_in_isr()) {
		return -EWOULDBLOCK;
	}

	k_mutex_lock(&self->lock, K_FOREVER);

	if (_mcp23s17_read_regs(dev, REG_INTFA, buf, sizeof(buf)) != 0) {
		LOG_ERR("Failed to read port registers");
		goto done;
	}

	ret = ((uint32_t)buf[1] << 8) | buf[0];

	self->regs.INTFA = buf[0];
	self->regs.INTFB = buf[1];
done:
	k_mutex_unlock(&self->lock);
	return ret;
}

static const struct gpio_driver_api _mcp23s17_api = {
	.pin_configure = _mcp23s17_pin_configure,
	.port_get_raw = _mcp23s17_port_get_raw,
	.port_set_masked_raw = _mcp23s17_port_set_masked_raw,
	.port_set_bits_raw = _mcp23s17_port_set_bits_raw,
	.port_clear_bits_raw = _mcp23s17_port_clear_bits_raw,
	.port_toggle_bits = _mcp23s17_port_toggle_bits,
	.pin_interrupt_configure = _mcp23s17_pin_interrupt_configure,
	.manage_callback = _mcp23s17_manage_callback,
	.get_pending_int = _mcp23s17_get_pending_int,
};

static void _mcp23s17_irq_work(struct k_work *work)
{
	struct mcp23s17 *self = CONTAINER_OF(work, struct mcp23s17, irq_work);
	uint8_t buf[2];

	k_mutex_lock(&self->lock, K_FOREVER);

	if (_mcp23s17_read_regs(self->dev, REG_INTFA, buf, sizeof(buf)) != 0) {
		LOG_ERR("Failed to read interrupt flag registers");
		goto done;
	}

	uint32_t intf = ((uint32_t)buf[1] << 8) | buf[0];

	self->regs.INTFA = buf[0];
	self->regs.INTFB = buf[1];

	gpio_fire_callbacks(&self->cb, self->dev, intf);

done:
	k_mutex_unlock(&self->lock);
}

static void _mcp23s17_irqa_callback(const struct device *dev, struct gpio_callback *gpio_cb,
				    uint32_t pins)
{
	struct mcp23s17 *self = CONTAINER_OF(gpio_cb, struct mcp23s17, inta_cb);

	ARG_UNUSED(pins);
	k_work_submit(&self->irq_work);
}

static void _mcp23s17_irqb_callback(const struct device *dev, struct gpio_callback *gpio_cb,
				    uint32_t pins)
{
	struct mcp23s17 *self = CONTAINER_OF(gpio_cb, struct mcp23s17, intb_cb);

	ARG_UNUSED(pins);
	k_work_submit(&self->irq_work);
}

static int _mcp23s17_init(const struct device *dev)
{
	if (!dev) {
		return -EINVAL;
	}

	struct mcp23s17 *self = (struct mcp23s17 *)dev->data;
	struct mcp23s17_config *config = (struct mcp23s17_config *)dev->config;

	memset(self, 0, sizeof(*self));

	self->dev = dev;

	sys_slist_init(&self->cb);
	k_mutex_init(&self->lock);
	k_work_init(&self->irq_work, _mcp23s17_irq_work);

	if (_mcp23s17_read_regs(dev, 0, (uint8_t *)&self->regs, sizeof(self->regs)) != 0) {
		LOG_ERR("Could not read registers");
		return -EIO;
	}

	if (self->regs.IODIRA != 0xff || self->regs.IODIRB != 0xff) {
		LOG_ERR("Expected IODIRA and IODIRB to have default value 0xff");
		return -EINVAL;
	}

	if (device_is_ready(config->inta_gpio.port)) {
		if (gpio_pin_configure_dt(&config->inta_gpio, GPIO_INPUT) != 0) {
			LOG_ERR("Error initializing INTA gpio");
			return -EIO;
		}
		if (gpio_pin_interrupt_configure_dt(&config->inta_gpio, GPIO_INT_EDGE_TO_ACTIVE) !=
		    0) {
			LOG_ERR("Error configuring interrupt on INTA gpio");
			return -EIO;
		}
		gpio_init_callback(&self->inta_cb, _mcp23s17_irqa_callback,
				   BIT(config->inta_gpio.pin));
		if (gpio_add_callback(config->inta_gpio.port, &self->inta_cb) != 0) {
			LOG_ERR("Error setting callback for INTA");
			return -EIO;
		}
	}

	if (device_is_ready(config->intb_gpio.port)) {
		if (gpio_pin_configure_dt(&config->intb_gpio, GPIO_INPUT) != 0) {
			LOG_ERR("Error initializing INTA gpio");
			return -EIO;
		}
		if (gpio_pin_interrupt_configure_dt(&config->intb_gpio, GPIO_INT_EDGE_TO_ACTIVE) !=
		    0) {
			LOG_ERR("Error configuring interrupt on INTA gpio");
			return -EIO;
		}
		gpio_init_callback(&self->intb_cb, _mcp23s17_irqb_callback,
				   BIT(config->intb_gpio.pin));
		if (gpio_add_callback(config->intb_gpio.port, &self->intb_cb) != 0) {
			LOG_ERR("Error setting callback for INTA");
			return -EIO;
		}
	}

	LOG_INF("MCP23S17 initialized (%p)", self->dev);
	return 0;
}

#define MCP23S17_INIT_PRIORITY 75
#define MCP23S17_INIT(n)                                                                           \
	static const struct mcp23s17_config _mcp23s17_##n##_config = {                             \
		.spi = SPI_DT_SPEC_INST_GET(                                                       \
			n, SPI_OP_MODE_MASTER | SPI_MODE_CPOL | SPI_MODE_CPHA | SPI_WORD_SET(8),   \
			0),                                                                        \
		.inta_gpio = GPIO_DT_SPEC_INST_GET_OR(n, inta_gpios, { 0 }),                       \
		.intb_gpio = GPIO_DT_SPEC_INST_GET_OR(n, intb_gpios, { 0 }),                       \
	};                                                                                         \
	static struct mcp23s17 _mcp23s17_##n##_data = { 0 };                                       \
	DEVICE_DT_INST_DEFINE(n, _mcp23s17_init, NULL, &_mcp23s17_##n##_data,                      \
			      &_mcp23s17_##n##_config, POST_KERNEL, MCP23S17_INIT_PRIORITY,        \
			      &_mcp23s17_api)

DT_INST_FOREACH_STATUS_OKAY(MCP23S17_INIT);
