# CMake generated Testfile for 
# Source directory: /home/victor/blockchain-sim/tests/regtest/rust/1_hello
# Build directory: /home/victor/blockchain-sim/cmake-build-debug/tests/regtest/rust/1_hello
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(BLEEP-regtest-rust-hello "python" "test.py" "--noserver" "rust/1_hello/rust_hello.xml")
set_tests_properties(BLEEP-regtest-rust-hello PROPERTIES  WORKING_DIRECTORY "/home/victor/blockchain-sim/tests/regtest/rust/1_hello/../.." _BACKTRACE_TRIPLES "/home/victor/blockchain-sim/tests/regtest/rust/1_hello/CMakeLists.txt;27;add_test;/home/victor/blockchain-sim/tests/regtest/rust/1_hello/CMakeLists.txt;0;")
