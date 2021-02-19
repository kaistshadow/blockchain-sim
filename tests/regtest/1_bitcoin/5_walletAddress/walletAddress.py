import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def get_info(xml_file):
    split_result = []
    split_result2 = []
    f = open(xml_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("kill time")
        if result != -1:
            split_result = line.split('"')
        result = line.find("node id")
        if result != -1:
            result = line.find("poi")
            if result == -1:
                split_result2 = line.split('"')
                break
    f.close()
    return split_result[1],split_result2[1]

def test_xml_existence():
    path = os.path.abspath(".")
    target_folder_xml = path + "/output.xml"
    if os.path.isfile(target_folder_xml):
        return target_folder_xml
    else:
        sys.exit(1)

# check log file
# todo : rpc-client data filter
def test_file_existence(node_id):
    path = os.path.abspath(".")
    target_folder_bitcoin = path + "/shadow.data/hosts/" + node_id + "/stdout-" + node_id + ".bitcoind.1000.log"
    target_folder_rpc = path + "/shadow.data/hosts/client0/stdout-client0.rpc.1000.log"
    if os.path.isfile(target_folder_bitcoin) & os.path.isfile(target_folder_rpc):
        return target_folder_bitcoin, target_folder_rpc
    else:
        sys.exit(1)

# check "getnewaddress" log in shadow output file. If done, check length of rpc_file contents. If the length is 35, the test is success.
def test_walletAddress(shadow_output_file, rpc_output_file):
    return_count = 0
    f = open(shadow_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("getnewaddress")
        if result != -1:
            return_count += 1
    f.close()
    if return_count == 0:
        sys.exit(1)

    f = open(rpc_output_file, "r")
    line = f.readline().strip()
    f.close()
    if len(line) == 35:
        sys.exit(0)
    else:
        sys.exit(1)

# test process
# 1. test_xml_existence 
# 2. shadow output existence test
# 3. wallet log test
def main():
    target_folder_xml = test_xml_existence()
    exec_shell_cmd("shadow output.xml")
    runtime, node_id = get_info(target_folder_xml)
    shadow_output_file, rpc_output_file = test_file_existence(node_id)
    test_walletAddress(shadow_output_file, rpc_output_file)

if __name__ == '__main__':
    main()