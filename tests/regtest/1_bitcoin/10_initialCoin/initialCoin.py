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

def test_initialCoin(simulation_output_file):
    f = open(simulation_output_file, "r")
    line = f.readline()
    f.close()
    block_count = int(line.split(",")[1].split(":")[1])
    if block_count > 0:
        print("Success InitalCoin test ...")
        sys.exit(0)
    else:
        print("Fail InitalCoin test ...")
        sys.exit(1)

# test process
# 1. test_xml_existence 
# 2. shadow output existence test
# 3. wallet log test
def main():
    # add xml generator - add initial coin logic 
    target_folder_xml = test_modules.test_xml_existence("output.xml")
    exec_shell_cmd("shadow output.xml")
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    test_initialCoin(simulation_output_file[1])
   

if __name__ == '__main__':
    main()