# CMake generated Testfile for 
# Source directory: /home/victor/blockchain-sim/tests/regtest/test-consensus
# Build directory: /home/victor/blockchain-sim/cmake-build-debug/tests/regtest/test-consensus
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(BLEEP-regtest-consensus-test "python" "test.py" "--noserver" "test-consensus/Consensus[pow-tree]-Gossip[SP]-200node-2sec.xml")
set_tests_properties(BLEEP-regtest-consensus-test PROPERTIES  WORKING_DIRECTORY "/home/victor/blockchain-sim/tests/regtest/test-consensus/.." _BACKTRACE_TRIPLES "/home/victor/blockchain-sim/tests/regtest/test-consensus/CMakeLists.txt;10;add_test;/home/victor/blockchain-sim/tests/regtest/test-consensus/CMakeLists.txt;0;")
