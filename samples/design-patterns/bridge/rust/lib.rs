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

mod server {
    use super::printk;

    pub struct Circle {
    }

    pub struct Square {
    }

    pub struct Point2D(i32, i32);

    impl Circle {
        pub fn new() -> Self {
            return Self { };
        }
    }

    impl Square {
        pub fn new() -> Self {
            return Self { };
        }
    }

    pub trait PhysicsObject {
        fn position(&self) -> Point2D;
    }

    impl PhysicsObject for Circle {
        fn position(&self) -> Point2D {
            printk("server: get position for circle\n");
            return Point2D(1, 1);
        }
    }

    impl PhysicsObject for Square {
        fn position(&self) -> Point2D {
            printk("server: get position for square\n");
            return Point2D(1, 1);
        }
    }
}

mod client {
    use super::server;
    use super::printk;

    pub struct Circle<P: server::PhysicsObject> {
        physics: P,
    }

    pub struct Square<P: server::PhysicsObject> {
        physics: P,
    }

    impl<P: server::PhysicsObject> Circle<P> {
        pub fn new(p: P) -> Self {
            return Self { physics: p };
        }
    }

    impl<P: server::PhysicsObject> Square<P> {
        pub fn new(p: P) -> Self {
            return Self { physics: p };
        }
    }

    pub trait Drawable {
        fn draw(&self);
    }

    impl<P: server::PhysicsObject> Drawable for Circle<P> {
        fn draw(&self){
            let _p = self.physics.position();
            printk("client: drawing circle at position\n");
        }
    }

    impl<P: server::PhysicsObject> Drawable for Square<P> {
        fn draw(&self){
            let _p = self.physics.position();
            printk("client: drawing square at position\n");
        }
    }
}

#[no_mangle]
pub extern "C" fn rust_main() -> usize {
    use client::{Drawable};
    printk("RUST main\n");
    let circle = client::Circle::new(server::Circle::new());
    let square = client::Square::new(server::Square::new());
    circle.draw();
    square.draw();

    return 0;
}

#[panic_handler]
pub fn panic_handler(_info: &PanicInfo) -> ! {
    loop {};
}
