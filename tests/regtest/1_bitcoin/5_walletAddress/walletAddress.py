import os
from subprocess import check_output
import argparse
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
import test_modules

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

# check "getnewaddress" log in shadow output file. If done, check length of rpc_file contents. If the length is 35, the test is success.
def test_walletAddress(simulation_output_file):
    return_count = 0
    f = open(simulation_output_file[0], "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("getnewaddress")
        if result != -1:
            return_count += 1
    f.close()
    if return_count == 0:
        print("[Fail wallet test] - bitcoind didn't works about getnewaddress")
        sys.exit(1)

    f = open(simulation_output_file[1], "r")
    line = f.readline().strip()
    f.close()
    if len(line) == 35:
        print("Success wallet test ...")
        sys.exit(0)
    else:
        print("[Fail wallet test] - wallet address length is not 35")
        sys.exit(1)

# test process
# 1. test_xml_existence 
# 2. shadow output existence test
# 3. wallet log test
def main():
    target_folder_xml = test_modules.test_xml_existence("output.xml")
    exec_shell_cmd("shadow output.xml")
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    test_walletAddress(simulation_output_file)

if __name__ == '__main__':
    main()