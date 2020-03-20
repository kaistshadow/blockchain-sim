# CMake generated Testfile for 
# Source directory: /home/victor/blockchain-sim/tests/regtest/test_simpleconnect
# Build directory: /home/victor/blockchain-sim/cmake-build-debug/tests/regtest/test_simpleconnect
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(BLEEP-regtest-simpleconnect "python" "test.py" "--noserver" "test_simpleconnect/simple-connect.xml")
set_tests_properties(BLEEP-regtest-simpleconnect PROPERTIES  WORKING_DIRECTORY "/home/victor/blockchain-sim/tests/regtest/test_simpleconnect/.." _BACKTRACE_TRIPLES "/home/victor/blockchain-sim/tests/regtest/test_simpleconnect/CMakeLists.txt;6;add_test;/home/victor/blockchain-sim/tests/regtest/test_simpleconnect/CMakeLists.txt;0;")
