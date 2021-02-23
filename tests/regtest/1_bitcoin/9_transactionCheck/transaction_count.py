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

# count "sendtoaddress" rpc request in bitcoin log and get transaction counts in tx injector log.
# If the two are the same, true
def test_transaction_count(simulation_output_file):
    txs_bitcoind = 0
    blocks_count = 0
    f = open(simulation_output_file[0], "r")
    for line in f.readlines()[::-1]:
        result = line.find("UpdateTip")
        if result != -1:
            split_list = line.split(" ")
            for i in range(0,len(split_list)):
                result = split_list[i].find("height=")
                if result != -1:
                    blocks_count = int(split_list[i].split("=")[1])
                    continue
                result = split_list[i].find("tx=")
                if result != -1:
                    txs_bitcoind = int(split_list[i].split("=")[1])
                    break
            if txs_bitcoind != 0:
                break    
    f.close()
    txs_bitcoind = txs_bitcoind - blocks_count - 1

    txs = 0
    f = open(simulation_output_file[0], "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("getmempoolinfo")
        if result != -1:
            break
        result = line.find("sendtoaddress")
        if result != -1:
            txs += 1
    f.close()

    f = open(simulation_output_file[1], "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("mempool")
        if result != -1:
            mempool_size = line.split(",")[1].split(":")[1]
            break
    f.close()

    if txs_bitcoind + int(mempool_size) == txs:
        sys.exit(0)
    else:
        sys.exit(1)

# Test process
# 1. test_xml_existence 
# 2. shadow output existence test  
# 3. Run test_transaction_existence function
def main():
    target_folder_xml = test_modules.test_xml_existence("output.xml")
    # exec_shell_cmd("shadow output.xml")
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    test_modules.test_transaction_existence(simulation_output_file[1])
    test_transaction_count(simulation_output_file)

if __name__ == '__main__':
    main()