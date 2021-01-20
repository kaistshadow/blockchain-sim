import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def test_node_init(node_log, standard_log):
    f = open(node_log, "r")
    result = []

    while True:
        line = f.readline()
        if not line: break
        for i in range(0,len(standard_log)):
            compare = standard_log[i] in line
            if compare == True:
                result.append(standard_log[i])
                break

    f.close()            
    return result

def test_node_connection(node_log, standard_log):
    f = open(node_log, "r")
    while True:
        line = f.readline()
        if not line: break
        if standard_log in line:
            return True
    return False

def init_test_start(blockchain_network_init_standard, blockchain_connection_standard, blockchain_network, node_count, abs_path):
    print("----------------------------------------------------------------------------------")
    print("                           %s node test" %(blockchain_network))
    print("----------------------------------------------------------------------------------")
    return_count = 0
    for i in range(0,node_count):
        count = 0
        print("")
        print("")
        print("----------------")
        print("Node%d start" %(i+1))
        print("----------------")
        print("")
        print("[Node initialization]")
        print("")
        target_file = abs_path + "bcdnode" + str(i+1)  + "/stdout-bcdnode" + str(i+1) + ".bitcoind.1000.log"
        target_result = test_node_init(target_file, blockchain_network_init_standard)
        for j in range(0,len(blockchain_network_init_standard)):
            if blockchain_network_init_standard[j] in target_result:
                print("\t%s\t                  ... Successfully" %(blockchain_network_init_standard[j]))
                count += 1
            else:
                return_count = 1
                print("\t%s\t                  ... failed" %(blockchain_network_init_standard[j]))
        print("")
        print("\t -> result : %d/%d  <-" %(count, len(blockchain_network_init_standard)))  
        print("")
        print("[Peer connection]")
        print("")
        if test_node_connection(target_file, blockchain_connection_standard) == True:
            print("\t ... Successfully about connection peer")
            print("")
        else:
            print("\t ... Failed connection peer")
            print("")
            return_count = 1
        print("-----------------------------")
    if return_count == 1:
        return -1
    else:
        return 0

def main():
    # ---------------[ Bitcoin test standard ]-------------------

    bitcoin_init_standard = ['scheduler thread start','torcontrol thread start','Starting network threads','addcon thread start', 'addcon thread start', 'msghand thread start', 'opencon thread start']
    bitcoin_connection_standard = "New outbound peer connected"
    # bitcoin_mining_standard = []
    # bitcoin_block_synchronization = 

    # -------------- [ Litecoin test standard ]-------------------

    litecoin_init_standard = []
    # bitcoin_connection_standard = []

    parser = argparse.ArgumentParser(description='Script for installation and simulation')

    # -------------------------------------------

    parser.add_argument("--bitcoin", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--litecoin", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--monero", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--eos", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--ethereum", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--zcash", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--ripple", action="store_true", help="Install the shadow simulator and BLEEP")

    args = parser.parse_args()

    # -------------------------------------------

    OPT_BITCOIN = args.bitcoin
    OPT_LITECOIN = args.litecoin
    OPT_MONERO = args.monero
    OPT_EOS = args.eos
    OPT_ETHEREUM = args.ethereum
    OPT_ZCASH = args.zcash
    OPT_RIPPLE = args.ripple

    # -------------------------------------------

    cmake_bitcoin_opt = "-BITCOIN_OPT=ON"
    cmake_litecoin_opt = "-LITECOIN_OPT=ON"
    cmake_monero_opt = "-MONERO_OPT=ON"
    cmake_eos_opt = "-EOS_OPT=ON"
    cmake_ethereum_opt = "-ETHEREUM_OPT=ON"
    cmake_zcash_opt = "-ZCASH_OPT=ON"
    cmake_ripple_opt = "-RIPPLE_OPT=ON"
    
    # -------------------------------------------
    a = './'
    abs_path = os.path.abspath(a)
    if OPT_BITCOIN:
        abs_path = abs_path + "/shadow-bitcoin/1_firstrun/shadow.data/hosts/"
        node_count = len(os.walk(abs_path).next()[1])
        return init_test_start(bitcoin_init_standard, bitcoin_connection_standard, "Bitcoin", node_count, abs_path)
    
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