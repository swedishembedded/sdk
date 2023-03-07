#![no_std]
use core::panic::PanicInfo;

extern "C" {
    fn serial_out(c: u8);
}

pub fn printk(msg: &str) {
    for c in msg.chars() {
        unsafe {
            serial_out(c as u8);
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_main() -> usize {
    printk("RUST main\n");

    return 0;
}

#[panic_handler]
pub fn panic_handler(_info: &PanicInfo) -> ! {
    loop {};
}
