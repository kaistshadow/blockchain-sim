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
    count = 0
    f = open(simulation_output_file[0], "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("sendtoaddress")
        if result != -1:
            count += 1
    f.close()
     
    f = open(simulation_output_file[2], "r")
    for line in f.readlines()[::-1]:
        result = line.find("transaction")
        if result != -1:
            txs = int(line.split(" ")[0])
            break
    f.close()

    if txs == count:
        sys.exit(0)
    else:
        sys.exit(1)

# Test process
# 1. test_xml_existence 
# 2. shadow output existence test  
# 3. Run test_transaction_existence function
def main():
    target_folder_xml = test_modules.test_xml_existence("output.xml")
    exec_shell_cmd("shadow output.xml")
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    test_modules.test_transaction_existence(simulation_output_file[1])
    test_transaction_count(simulation_output_file)

if __name__ == '__main__':
    main()