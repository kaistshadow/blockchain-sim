#[no_mangle]
pub extern "C" fn main() -> i32 {
    println!("main in lib.rs called\n");
    // rust does not use implicit return code on the main, but Shadow requires return code for the plugin's main function.

    
    return 0
    // std::process::exit(0) // It also results in abort error on github action.
    // 0 // return value, but this results in abort error on github action.
}