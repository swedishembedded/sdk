# MCP23S17 Sample
This sample demonstrates use of example sensor which is implemented in Renode.

## Building

This sample can be built and executed on renode as follows:

```bash
./scripts/build
renode renode/samples/drivers/example/example_sensor/custom_board.resc
```

## Running
Once renode console is active, you can simulate new samples being available by
executing:

```bash
start
sysbus.spi1.exampleSensor Temperature 12.0
```

The UART console should then print the temperature received by the sample.
