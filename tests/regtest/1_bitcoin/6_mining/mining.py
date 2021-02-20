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

# "height>0" means mining is activated and works good.
# If "height>0" is found at bitcoin log, mining works. 
def test_mining(shadow_output_file):
    f = open(shadow_output_file[0], "r")
    while True:
        line = f.readline()
        if not line: break
        # height=1 means mining activated.
        result = line.find("height=0")
        if result != -1:
            f.close()
            print("Success mining test ...")
            sys.exit(0)
    f.close()
    print("Fail mining test ...")
    sys.exit(1)
    
# Test process
# 1. test_xml_existence 
# 2. shadow output existence test  
# 3. Run test_mining function
def main():
    target_folder_xml = test_modules.test_xml_existence()
    exec_shell_cmd("shadow output.xml")
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    test_mining(simulation_output_file)

if __name__ == '__main__':
    main()