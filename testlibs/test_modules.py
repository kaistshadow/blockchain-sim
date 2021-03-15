import os
from subprocess import check_output
import argparse
import sys
import os
import lxml.etree as ET
import subprocess
import math
sys.path.append("")
from testlibs import utils

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)


# Regression test-01 - xmlGenerate test
# xml existence test - If there is no file in the path, return fail
def test_xml_existence(output):
    path = os.path.abspath(".")
    target_folder = path + "/" + output
    if os.path.isfile(target_folder):
        print("Success xml existence test ...")
        return target_folder
    else:
        print("Fail xml existence test ...")
        sys.exit(1)

# Regression test-02-1 - shadow test         
def test_shadow_output_file_existence(condition_number, node_id_list):
    # plugin output check
    for i in range(0,len(node_id_list)):
        path_ = os.path.abspath(".")
        path_ = path_ + "/shadow.data/hosts/" + node_id_list[i]
        if os.path.isdir(path_):
            pass
        else:
            print("%s file not existence shadow output file ... " %node_id_list[i])
            sys.exit(1)

    # shadow output.txt
    if condition_number == "regtest":
        path = os.path.abspath(".")
    else:
        path = os.path.abspath("./shadow.data")
    target_folder_file = path + "/output.txt"
    if os.path.isfile(target_folder_file):
        return target_folder_file
    else:
        print("Fail not existence shadow output file ... ")
        sys.exit(1)

# Regression test-02-2 - shadow test
# test1 : whether runtime setting worked or not 
# test2 : whether plugin(node_id) worked or not
def test_shadow(output_file, runtime, node_id_list, shadow_output):
    f = open(output_file, "r")
    # result_count more than 3 means success.
    result_count = 0
    return_count = 0
    return_time = utils.get_time_form(runtime)
    while True:
        line = f.readline()
        if not line: break
        result = line.find("_process_start")
        if result != -1:
            result = line.find("has set up the main pth thread")
            if result != -1:
                for i in range(0,len(node_id_list)):
                    result = line.find(node_id_list[i])
                    if result != -1:
                        result_count = 1
              
        result = line.find(return_time)
        if result != -1:
            if result_count == 1:
                f.close()
                print("Success shadow test ...")
                return_count = 1
                break
            else:
                f.close()
                print("Fail shadow test] - runtime error ...")
                sys.exit(1)

    if return_count == 0:            
        f.close()
        print("[Fail shadow test] - plugin does not run ... ")
        utils.filter_fail_shadow_test(shadow_output)
        sys.exit(1)
    else:
        pass

# Regression test-02-2 - shadow test (emulation ver)
# test1 : whether runtime setting worked or not 
# test2 : whether plugin(node_id) worked or not'
def emul_test_shadow(output_file, runtime, node_id_list, shadow_output):
    complete_node = []
    f = open(output_file, "r")
    # result_count more than 3 means success.
    result_count = 0
    return_time = utils.get_time_form(runtime)
    while True:
        line = f.readline()
        if not line: break
        result = line.find("_process_start")
        if result != -1:
            result = line.find("has set up the main pth thread")
            if result != -1:
                result = line.find("bitcoind")
                if result != -1:
                    complete_node.append(line.split(" ")[4].split("~")[1][:-1])
                for i in range(0,len(node_id_list)):
                    result = line.find(node_id_list[i])
                    if result != -1:
                        result_count = 1
              
        result = line.find(return_time)
        if result != -1:
            if result_count == 1:
                f.close()
                print("Success shadow test ...")
                return complete_node, runtime
            else:
                f.close()
                print("Fail shadow test] - runtime error ...")
                utils.filter_fail_shadow_test(shadow_output)
                sys.exit(1)
    f.close()
    print("[Fail shadow test] - plugin does not run ...")
    sys.exit(1)


# Regression test-03-1 - bitcoinApplication file existence test.
# After shadow, check output data
def test_file_existence(node_id_list, plugin_list):
    if len(node_id_list) != len(plugin_list):
        sys.exit(1)
    path = os.path.abspath(".")
    target_folder_list = []
    for i in range(0,len(node_id_list)):
        target_path = path + "/shadow.data/hosts/" + node_id_list[i] + "/stdout-" + node_id_list[i] + "." + plugin_list[i] + ".1000.log"
        target_folder_list.append(target_path)
    for i in range(0,len(target_folder_list)):
        if os.path.isfile(target_folder_list[i]) == False:
            print("Fail not existence file - %s" %(target_folder_list[i]))
            sys.exit(1)
    print("Success blockchain test output file existence ...")
    return target_folder_list

# Regression test-03-2 - bitcoinApplication test
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

# Regression test-04 - bitcoin difficulty test
# 설정한 난이도와 시뮬레이션된 비트코인 난이도가 일치한지 테스트
def test_difficulty_compare(bitcoin_log, xml_difficulty):
    difficulty = ""
    f = open(bitcoin_log, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("difficulty")
        if result != -1:
            split_list = line.split(",")[4]
            difficulty = split_list.split(":")[1]
            break
    f.close()

    if str(xml_difficulty) == "1":
        if difficulty == "1":
            print("Success difficulty test ...")
            sys.exit(0)
        else:
            print("Fail difficulty test ...")
            sys.exit(1)

    elif str(xml_difficulty) == "2":
        if difficulty == "0.00390625":
            print("Success difficulty test ...")
            sys.exit(0)
        else:
            print("Fail difficulty test ...")
            sys.exit(1)
        # error case                                

    elif str(xml_difficulty) == "3":
        if difficulty == "0.0002441371325370145":
            print("Success difficulty test ...")
            sys.exit(0)
        else:
            print("Fail difficulty test ...")
            sys.exit(1)

# Regression test-05 - wallet address test
# bitcoin-cli validateaddress call
# Return information about the given bitcoin address.
def test_walletAddress(simulation_output_file):
    the_wallet_address = ""
    f = open(simulation_output_file[1], "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("isvalid")
        if result != -1:
            the_wallet_validation = line.split(",")[0].split('"')[4].split(":")[1]
            if the_wallet_validation == "true":
                print("Success wallet validation test ... ")
                break
            else:
                print("Fail wallet validation test ... ")
                sys.exit(1)

# Regression test-06 - mining test
# "height>0" means mining is activated and works good.
# If "height>0" is found at bitcoin log, mining works. 
def test_mining(shadow_output_file):
    f = open(shadow_output_file[0], "r")
    while True:
        line = f.readline()
        if not line: break
        # height=1 means mining activated.
        result = line.find("height=1")
        if result != -1:
            f.close()
            print("Success mining test ...")
            sys.exit(0)
    f.close()
    print("Fail mining test ...")
    sys.exit(1)

# Regression test-07 - bitcoin mainchain test
# Get "bestblockchash" value info using rpc.output_file then check for the same value in bitcoin output log.
def test_MainChainInfo(shadow_output_file, rpc_output_file):
    return_count = 0
    f = open(rpc_output_file , "r")
    for line in f.readlines()[::-1]:
        line.rstrip()
        result = line.find("bestblockhash")
        if result != -1:
            line = line.split(",")[3].split(":")[1]
            genesisHash = line[1:len(line)-1]
            return_count = 1
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

# Regression test-08 - transaction test
# If rpc output file has error log, the transaction is not created properlys
def test_transaction_existence(simulation_output_file):
    return_count = 0
    f = open(simulation_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find('"error":null')
        if result != -1:
            print("Success transaction test ...")
            return_count = 1
            break
    f.close()
    if return_count == 0:
        print("Fail transaction test ...")
        sys.exit(1)
    else:
        pass

# Regression test-09 - transaction count test 
# 설정한 트랜잭션의 개수와 생성되 트랜잭션의 개수를 비교하는 테스트
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
    return txs_bitcoind

# Regression test-10 - initial coin test 
# initial coin이 생성된지 확인하기 위한 테스트.
def test_initialCoin(simulation_output_file):
    f = open(simulation_output_file, "r")
    block_count = 0
    
    for line in f.readlines()[::-1]: 
        
        result = line.find("bestblockhash")
        if result != -1:
            f.close()
            split_list = line.split(",")
            for i in range(0,len(split_list)):
                result = split_list[i].find("blocks")
                if result != -1:
                    block_count = int(split_list[i].split(":")[1])
                    break
        if block_count != 0:
            f.close()
            break
            
    if block_count > 0:
        print("Success InitalCoin test ...")
        sys.exit(0)
    else:
        print("Fail InitalCoin test ...")
        sys.exit(1)