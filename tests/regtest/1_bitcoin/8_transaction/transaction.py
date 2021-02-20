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

# If rpc output file has error log, the transaction is not created properly
def test_transaction_existence(simulation_output_file):
    f = open(simulation_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find('result":null,"error')
        if result != -1:
            f.close()
            print("Success transaction test ...")
            sys.exit(1)
    f.close()
    print("Fail transaction test ...")
    sys.exit(0)

# Test process
# 1. test_xml_existence 
# 2. shadow output existence test  
# 3. Run test_transaction_existence function
def main():
    target_folder_xml = test_modules.test_xml_existence()
    exec_shell_cmd("shadow output.xml")
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    test_transaction_existence(simulation_output_file[1])

if __name__ == '__main__':
    main()