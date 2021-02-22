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

def test_args(args_standard):
    for i in range(0,len(args_standard)):
        if args_standard[i] is None:
            sys.exit(1)

# This test compares the shadow result log with standard args, 
# and succeeds if all of the args are in the shadow result log.    
def test_bitcoinApplication(output_file, args_standard):
    j = 0
    f = open(output_file[0], "r")
    while True:
        line = f.readline()
        if not line: break
        for i in range(j,len(args_standard)):
            result = line.find(args_standard[i])
            if result != -1:
                j += 1
    f.close()
    if j == len(args_standard):
        print("Success bitcoin application test ...")
        sys.exit(0)
    else:
        print("Fail bitcoin application test ...")
        sys.exit(1)

def main():
    # difficulty setting
    # mining algorithm option setting
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--rpcbind", metavar="rpcbind", help="blockchain application args")
    parser.add_argument("--rpcallowip", metavar="rpcallowip", help="blockchain application args")
    parser.add_argument("--rpcport", metavar="rpcport",  help="blockchain application args ")
    parser.add_argument("--rpcpassword", metavar="rpcpassword", default="1234", help="simulblockchain plugin node rpcpassword ")
    parser.add_argument("--rpcuser", metavar="rpcuser", default="a", help="simulblockchain plugin node rpcuser")
    parser.add_argument("--port", metavar="port", help="blockchain plugin node port")
    parser.add_argument("--datadir", metavar="datadirection", help="blockchain plugin node datadirection")
    args = parser.parse_args()
    args_standard = []

    rpcbind = args.rpcbind
    rpcallowip = args.rpcallowip
    rpcpassword = args.rpcpassword
    rpcuser = args.rpcuser

    # args standards
    rpcport = args.rpcport
    port = args.port
    datadir = args.datadir
    
    args_standard.append(rpcport)
    args_standard.append(port)
    # args_standard.append(datadir)

    test_args(args_standard)
    target_folder_xml = test_modules.test_xml_existence("output.xml")
    # todo - output.xml file must be made using above args.
    # exec_shell_cmd("shadow output.xml")
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    test_bitcoinApplication(simulation_output_file ,args_standard)

if __name__ == '__main__':
    main()