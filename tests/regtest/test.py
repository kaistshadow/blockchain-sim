import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def test_node_init(node_log, standard_log):
    result = []
    for i in range(0,len(standard_log)):
        f = open(node_log, "r")
        while True:
            line = f.readline().rstrip("\n")
            if not line:
                f.close()
                break
            compare = standard_log[i] in line
            if compare == True:
                result.append(standard_log[i])
                f.close()
                break
    if 1 == len(result)/len(standard_log):
        return 0
    else:
        return -1

# Different blockchain platforms have different paths
# Such as bitcoin dir1/dir2/dir3/data,
# Litecoin dir1/debug.log data  
# So must be classified by platform
# This function returns log data file path + file name 's list
def set_file_name(blockchain, first_dir_name, second_dir_name, thrid_dir_name, node_count, abs_path):

    return_list = []

    if blockchain == "Bitcoin":
        for i in range(0,node_count):
            target_file = abs_path + first_dir_name + str(i+1) +second_dir_name + str(i+1) + thrid_dir_name
            return_list.append(target_file)

    if blockchain == "Monero":
        pass

    return return_list

def compare_data(blockchain_standard_list, file_name_list):
    
    for i in range(0,len(file_name_list)):
        for j in range(0,len(blockchain_standard_list)):
            if test_node_init(file_name_list[i], blockchain_standard_list[j]) == 0:
                continue
            else:
                return -1
    return 0

def init_test_start(blockchain_standard_list, blockchain_network, node_count, abs_path):

    if blockchain_network == "Bitcoin":
        file_list = set_file_name("Bitcoin", "bcdnode", "/stdout-bcdnode", ".bitcoind.1000.log", node_count, abs_path)
    if blockchain_network == "Monero":
        pass

    return compare_data(blockchain_standard_list, file_list)


# n parameter means the number of reading file such as node initialzation standard, peer connection standard, wallet standard etc ... 
# n == 2 : node initialzation standard, peer connection standard
# n == 3 : node initialzation standard, peer connection standard, wallet standard 
def file_read(abs_file_path, n):
    
    return_list = []
    # ---------------------------------------------------------------------- if you want to add some needed to check file, write down like below
    path_init = abs_file_path + "/initialization.txt"
    path_peer = abs_file_path + "/peerconnection.txt"

    standard_list = []
    peer_connection_list = []

    f = open(path_init,"r")
    while True:
        line = f.readline().rstrip("\n")
        standard_list.append(line)
        if not line: break
    f.close()
    return_list.append(standard_list)

    f = open(path_peer, "r")
    while True:
        line = f.readline().rstrip("\n")
        peer_connection_list.append(line)
        if not line: break
    f.close()
    return_list.append(peer_connection_list)

    # ---------- Only bitcoin has wallet enable. so other blockchains can emit this logic. 
    if n == 3:
        path_wallet = abs_file_path + "/wallet.txt"
        wallet_list = []
        f = open(path_wallet, "r")
        while True:
            line = f.readline().rstrip("\n")
            wallet_list.append(line)
            if not line: break
        f.close()
        return_list.append(wallet_list)

    return return_list


def main():

    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    a = './'
    abs_path = os.path.abspath(a)

    # -------------------------------------------

    parser.add_argument("--bitcoinfirstrun", action="store_true", help="Bitcoin first_run test output log check")
    parser.add_argument("--bitcoinwallet", action="store_true", help="Bitcoin wallet test output log check")


    parser.add_argument("--litecoin", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--monero", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--eos", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--ethereum", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--zcash", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--ripple", action="store_true", help="Install the shadow simulator and BLEEP")

    args = parser.parse_args()

    # -------------------------------------------

    OPT_BITCOIN_FIRSTRUN = args.bitcoinfirstrun
    OPT_BITCOIN_WALLET = args.bitcoinwallet


    OPT_LITECOIN = args.litecoin
    OPT_MONERO = args.monero
    OPT_EOS = args.eos
    OPT_ETHEREUM = args.ethereum
    OPT_ZCASH = args.zcash
    OPT_RIPPLE = args.ripple   

    # -------------------------------------------

    if OPT_BITCOIN_FIRSTRUN:
        abs_file_path = abs_path + "/shadow-bitcoin/1_firstrun"
        blockchain_standard_list = file_read(abs_file_path,2)
        abs_path = abs_path + "/shadow-bitcoin/1_firstrun/shadow.data/hosts/"
        node_count = len(os.walk(abs_path).next()[1])
        return init_test_start(blockchain_standard_list, "Bitcoin", node_count, abs_path)
    
    if OPT_BITCOIN_WALLET:
        abs_file_path = abs_path + "/shadow-bitcoin/2_with-wallet"
        blockchain_standard_list = file_read(abs_file_path,3)
        abs_path = abs_path + "/shadow-bitcoin/2_with-wallet/shadow.data/hosts/"
        node_count = len(os.walk(abs_path).next()[1])
        return init_test_start(blockchain_standard_list, "Bitcoin", node_count, abs_path)

    if OPT_LITECOIN:
        pass
    
    if OPT_MONERO:
        pass

    if OPT_EOS:
        pass

    if OPT_ETHEREUM:
        pass
    
    if OPT_ZCASH:
        pass

    if OPT_RIPPLE:
        pass

if __name__ == '__main__':
    main()