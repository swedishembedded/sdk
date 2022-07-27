# Example driver sample
This sample demonstrates use of example driver.

## Building

This sample can be built and executed on renode as follows:

```bash
west build -p -b native_posix samples/drivers/example/example_driver/ -t run
```

## Running

```bash
./build/zephyr/zephyr.elf

Example driver initialized!
[00:00:00.000,000] <inf> example_driver: Example driver initialized!
*** Booting Zephyr OS build zephyr-v3.0.0  ***
Sample started (v0.0.1)
```

You should see "Example driver initialized" printed in the console.

