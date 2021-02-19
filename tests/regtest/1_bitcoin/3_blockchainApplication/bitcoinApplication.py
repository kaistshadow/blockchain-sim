import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def test_xml_existence():
    path = os.path.abspath(".")
    target_folder_xml = path + "/output.xml"
    if os.path.isfile(target_folder_xml):
        return target_folder_xml
    else:
        sys.exit(1)

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

# check log file
# todo : rpc-client data filter
def test_file_existence(node_id):
    path = os.path.abspath(".")
    target_folder_bitcoin = path + "/shadow.data/hosts/" + node_id + "/stdout-" + node_id + ".bitcoind.1000.log"
    if os.path.isfile(target_folder_bitcoin):
        return target_folder_bitcoin
    else:
        sys.exit(1)

def test_args(args_standard):
    for i in range(0,len(args_standard)):
        if args_standard[i] is None:
            sys.exit(1)
    
def test_bitcoinApplication(output_file, args_standard):
    j = 0
    f = open(output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        for i in range(j,len(args_standard)):
            result = line.find(args_standard[i])
            if result != -1:
                j += 1
    f.close()
    if j == len(args_standard):
        sys.exit(0)
    else:
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

    rpcport = args.rpcport
    port = args.port
    datadir = args.datadir
    
    args_standard.append(rpcport)
    args_standard.append(port)
    # args_standard.append(datadir)

    test_args(args_standard)
    target_folder_xml = test_xml_existence()
    # todo - output.xml file must be made using above args.
    exec_shell_cmd("shadow output.xml")
    runtime, node_id = get_info(target_folder_xml)
    shadow_output_file = test_file_existence(node_id)
    test_bitcoinApplication(shadow_output_file ,args_standard)

if __name__ == '__main__':
    main()