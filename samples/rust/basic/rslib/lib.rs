#![no_std]
use core::panic::PanicInfo;

extern "C" {
    fn print_message_from_c();
    fn printk(fmt: *const u8, ...);
}

#[no_mangle]
pub extern "C" fn print_message_from_rust() -> () {
    let v = 123;
    unsafe {
        printk("Message from RUST: Hello World! %d\n".as_ptr(), v);
        print_message_from_c();
    }
}

#[panic_handler]
pub fn panic_handler(_info: &PanicInfo) -> ! {
    loop {};
}
