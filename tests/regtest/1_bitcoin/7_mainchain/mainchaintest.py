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

# Get "bestblockchash" value info using rpc.output_file then check for the same value in bitcoin output log.
def test_MainChainInfo(shadow_output_file, rpc_output_file):
    f = open(rpc_output_file , "r")
    line = f.readline()
    f.close()
    line_list = line.split(":")
    for i in range(0,len(line_list)):
        result = line_list[i].find("bestblockhash")
        if result != -1:
            return_count = 1
            genesisHash_sub = line_list[i+1].split('"')
            genesisHash = genesisHash_sub[1]
            break
    if return_count == 0:
        sys.exit(1)
    if os.path.isfile(shadow_output_file):
        f = open(shadow_output_file, "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find(genesisHash)
            if result != -1:
                print("Success MainChain test ...")
                sys.exit(0)
    else:
        print("Fail MainChain test ...")
        sys.exit(1)

# Test process
# 1. test_xml_existence 
# 2. shadow output existence test  
# 3. Run test_MainChainInfo function
def main():
    target_folder_xml = test_modules.test_xml_existence("output.xml")
    exec_shell_cmd("shadow output.xml")
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    test_MainChainInfo(simulation_output_file[0], simulation_output_file[1])

if __name__ == '__main__':
    main()