zephyr_library()

zephyr_library_sources_ifdef(CONFIG_EXAMPLE_DRIVER example_driver.c)
