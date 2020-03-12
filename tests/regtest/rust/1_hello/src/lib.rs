#[no_mangle]
pub extern fn main() {
    println!("main in lib.rs called\n");
    // rust does not use implicit return code on the main, but Shadow requires return code for the plugin's main function.
    std::process::exit(0);
}