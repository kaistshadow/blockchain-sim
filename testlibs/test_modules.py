#
# 2021-03-15
# created by hong joon
#
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

def test_result(condition_count, node_count, test_name):
    if condition_count == node_count:
        print("Success %s ... " %test_name)
        print("test result : %d/%d " %(condition_count,node_count))
    else:
        print("Fail %s ..." %test_name)
        print("test result : %d/%d " %(condition_count,node_count))
        sys.exit(1)

# --------------------------------------------------------------------------------------------------------------
#                                   Regression test-01 - xmlGenerate test        
# --------------------------------------------------------------------------------------------------------------
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

# --------------------------------------------------------------------------------------------------------------
#                                   Regression test-02-1 - shadow test         
# --------------------------------------------------------------------------------------------------------------
def test_shadow_output_file_existence(condition_number, node_id_list):
    # plugin output check
    for i in range(0,len(node_id_list)):
        path_ = os.path.abspath(".")
        path_ = path_ + "/shadow.data/hosts/" + node_id_list[i]
        if os.path.isdir(path_):
            pass
        else:
            print("%s file not existence shadow output file ... " %node_id_list[i])

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

# --------------------------------------------------------------------------------------------------------------
#                                   Regression test-02-2 - shadow test
# --------------------------------------------------------------------------------------------------------------
# test1 : whether runtime setting worked or not 
# test2 : whether plugin(node_id) worked or not
def test_shadow(output_file, runtime, node_id_list, shadow_output):
    f = open(output_file, "r")
    # result_count more than 3 means success.
    result_count = 0
    return_count = 0
    condition_runtime = 0
    return_time = utils.get_time_form(runtime)
    while True:
        line = f.readline()
        if not line: break
        result = line.find("has set up the main pth thread")
        if result != -1:
            result = line.find("_process_start")
            if result != -1:
                for i in range(0,len(node_id_list)):
                    result = line.find(node_id_list[i])
                    if result != -1:
                        result_count += 1
                        break
        
        # 설정한 runtime 동작 확인.
        if condition_runtime == 0:
            result = line.find(runtime)
            if result != -1:
                condition_runtime = 1

        result = line.find("starting to shut down")
        if result != -1:
            if result_count == len(node_id_list):
                f.close()
                print("Success shadow test ...")
                return_count = 1
                break
            else:
                f.close()
                print("shadow plugin error...")
                sys.exit(1)

    if return_count == 0:            
        f.close()
        if result_count == len(node_id_list):
            print("[shadow test] - runtime over ...  ")
            pass
        else:
            print("shadow runtime error .... ")
            utils.filter_fail_shadow_test(shadow_output)
            sys.exit(1)
    else:
        if condition_runtime != 1:
            print("Fail shadow test - runtime fail ... ")
            sys.exit(1)
        pass

# --------------------------------------------------------------------------------------------------------------
#                                   Regression test-02-2 - shadow test (emulation ver)
# --------------------------------------------------------------------------------------------------------------
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

# --------------------------------------------------------------------------------------------------------------
#                                   Regression test-03-1 - bitcoinApplication file existence test.
# --------------------------------------------------------------------------------------------------------------
# After shadow, check output data
def test_file_existence(node_id_list, plugin_list):
    if len(node_id_list) != len(plugin_list):
        sys.exit(1)
    path = os.path.abspath(".")
    target_folder_list = []
    target_error_list = []
    error_list = []
    for i in range(0,len(node_id_list)):
        target_path = path + "/shadow.data/hosts/" + node_id_list[i] + "/stdout-" + node_id_list[i] + "." + plugin_list[i] + ".1000.log"
        target_folder_list.append(target_path)
        target_path = path + "/shadow.data/hosts/" + node_id_list[i] + "/stderr-" + node_id_list[i] + "." + plugin_list[i] + ".1000.log"
        target_error_list.append(target_path)
    
    for i in range(0,len(target_folder_list)):
        if os.path.isfile(target_folder_list[i]) == False:
            print("Fail not existence shadow plugin output file - %s" %(target_folder_list[i]))
            sys.exit(1)
        # 에러 파일이 존재하면 그 파일의 path를 error_list에 append.
        if os.path.isfile(target_error_list[i]) == True:
            error_list.append(target_error_list[i])

    # 에러 파일이 존재하는 경우
    if len(error_list) != 0:
        print("Fail shadow plugin running ...")
        print("-------------- shadow plugin error contents --------------")
        for i in range(0,len(error_list)):
            f = open(error_list[i], "r")
            while True:
                line = f.readline().strip()
                print(line)
                if not line: break
            f.close()
        sys.exit(1)

    print("Success blockchain test output file existence ...")
    return target_folder_list

# --------------------------------------------------------------------------------------------------------------
#                                   Regression test-03-2 - bitcoinApplication test
# --------------------------------------------------------------------------------------------------------------
# This test compares the shadow result log with standard args, 
# and succeeds if all of the args are in the shadow result log.    
def test_bitcoinApplication(output_file, args_standard, node_count):
    j = 0
    condition_count = 0
    for z in range(0,int(node_count)):
        f = open(output_file[z], "r")
        while True:
            line = f.readline()
            if not line: break
            for i in range(j,len(args_standard)):
                result = line.find(args_standard[i])
                if result != -1:
                    j += 1
        f.close()
        if j == len(args_standard):
            condition_count += 1
        
    test_result(condition_count, node_count, "test_bitcoinApplication")
    sys.exit(0)

# --------------------------------------------------------------------------------------------------------------
#                                   Regression test-04 - bitcoin difficulty test
# --------------------------------------------------------------------------------------------------------------
# 설정한 난이도와 시뮬레이션된 비트코인 난이도가 일치한지 테스트
def test_difficulty_compare(bitcoin_log, xml_difficulty, node_count):
    condition_count = 0
    for z in range(0,node_count):
        difficulty = ""
        f = open(bitcoin_log[z + node_count], "r")
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
                condition_count += 1

        elif str(xml_difficulty) == "2":
            if difficulty == "0.00390625":
                condition_count += 1

        elif str(xml_difficulty) == "3":
            if difficulty == "0.0002441371325370145":
                condition_count += 1

    test_result(condition_count, node_count, "test_difficulty_compare test")
    sys.exit(0)

# --------------------------------------------------------------------------------------------------------------
#                                   Regression test-05 - wallet address test
# --------------------------------------------------------------------------------------------------------------
# bitcoin-cli validateaddress call
# Return information about the given bitcoin address.
def test_walletAddress(simulation_output_file, node_count):
    condition_count = 0
    for z in range(0,node_count):
        f = open(simulation_output_file[z+node_count], "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find("isvalid")
            if result != -1:
                the_wallet_validation = line.split(",")[0].split('"')[4].split(":")[1]
                if the_wallet_validation == "true":
                    condition_count += 1
                    continue

    test_result(condition_count, node_count, "test_walletAddress ")

# --------------------------------------------------------------------------------------------------------------
#                                   Regression test-06 - mining test
# --------------------------------------------------------------------------------------------------------------
# "height>0" means mining is activated and works good.
# If "height>0" is found at bitcoin log, mining works. 
def test_mining(shadow_output_file, node_count):
    condition_count = 0
    for z in range(0,node_count):
        f = open(shadow_output_file[z], "r")
        while True:
            line = f.readline()
            if not line: break
            # height=1 means mining activated.
            result = line.find("height=8")
            if result != -1:
                condition_count += 1
                break

    test_result(condition_count, node_count, "mining test")
    sys.exit(0)

# --------------------------------------------------------------------------------------------------------------
#                           Regression test-07 - bitcoin mainchain test 
# --------------------------------------------------------------------------------------------------------------
# Get "bestblockchash" value info using rpc.output_file then check for the same value in bitcoin output log.
def test_MainChainInfo(shadow_output_file, rpc_output_file, node_count):
    pork_count = 0
    while True:
        condition_count = utils.get_last_hashValue(shadow_output_file, rpc_output_file, node_count, pork_count)    
        if condition_count == node_count:
            break
        else:
            pork_count += 1

    if pork_count > 6:
        print("Fail mainchain test ... (network separation) ")
    elif pork_count == 0:
        pass
    else:
        print("There is a fork ...")
        condition_count = node_count

    test_result(condition_count, node_count, "mainchain test")
    sys.exit(0)

# --------------------------------------------------------------------------------------------------------------
#                           Regression test-08 - transaction test
# --------------------------------------------------------------------------------------------------------------
# If rpc output file has error log, the transaction is not created properlys
def test_transaction_existence(simulation_output_file, node_count):
    condition_count = 0
    for z in range(0,node_count):
        f = open(simulation_output_file[z+node_count], "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find('"error":null')
            if result != -1:
                print("Success transaction test ...")
                condition_count += 1
                break
        f.close()

    test_result(condition_count, node_count, "transaction test")

# --------------------------------------------------------------------------------------------------------------
#                           Regression test-09 - transaction count test (emulation ver)
# --------------------------------------------------------------------------------------------------------------
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

# --------------------------------------------------------------------------------------------------------------
#                           Regression test-09 - transaction count test (regtest ver)
# --------------------------------------------------------------------------------------------------------------
# count "sendtoaddress" rpc request in bitcoin log and get transaction counts in tx injector log.
# If the two are the same, true
def test_transaction_count_regtest(simulation_output_file, node_count):
    txs_bitcoind = 0
    blocks_count = 0
    mempool_size = 0
    condition_count = 0
    for z in range(0,node_count):
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
        txs_bitcoind = txs_bitcoind - blocks_count - 1

        txs = 0
        f = open(simulation_output_file[(node_count*2)], "r")
        for line in f.readlines()[::-1]:
            result = line.find('"error":null')
            if result != -1:
                txs += 1
            result = line.find('"result":null')
            if result != -1:
                break

        f = open(simulation_output_file[z + node_count], "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find("maxmempool")
            if result != -1:
                mempool_size = line.split(",")[1].split(":")[1]
                break
        f.close()

        if txs_bitcoind + int(mempool_size) == txs:
            condition_count += 1

    test_result(condition_count, node_count, "transaction count test")

# --------------------------------------------------------------------------------------------------------------
#                                       Regression test-10 - initial coin test 
# --------------------------------------------------------------------------------------------------------------
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

# --------------------------------------------------------------------------------------------------------------
#                                       Regression test-11 - peer connection test
# --------------------------------------------------------------------------------------------------------------
# bitcoin 기준으로 테스트를 한다고 할 경우, xml에 "addnode" 플래그 값으로 connection할 peer를 설정을 해주고, 설정해준 ip주소와
# "getpeerinfo"를 통해 시뮬레이션 상에서 connection된 peer ip와 일치하는지 확인하는 테스트. 
def test_peer_connection(plugin_output_files, IP_list, xml_file):
    addnode_list = utils.get_addnode_list(IP_list, xml_file)
    getpeerinfo_list = utils.get_peerinfo(plugin_output_files, IP_list)
    result_count = 0
    con_count = 0

    for i in range(0,len(addnode_list)):
        for j in range(0,len(addnode_list[i])):
            if addnode_list[i][j] in getpeerinfo_list[i]:
                con_count += 1
        if len(addnode_list[i]) == con_count:
            result_count += 1
        con_count = 0

    if result_count == len(addnode_list):
        print("Success peer connection test ...")
        sys.exit(0)
    else:
        print("fail peer connection test ...")
        sys.exit(1)

# --------------------------------------------------------------------------------------------------------------
#                                       Regression test-12 - load data file test
# --------------------------------------------------------------------------------------------------------------
# 1. 플러그인 로그에서 "Reindexing finished" 로그가 없으면 test 실패
# 2. coinlip_hash.txt 파일에서 블록 hash 값들을 읽어서, 플러그인의 로그에서 1~7번의 블록 hash 값과 비교
def test_dumpfile_load(plugin_output_files, abs_path, difficulty):
    print("dump test start ... ")
    condition_count = 0
    the_path = ""
    ready_hash_list = []
    f = open(plugin_output_files, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("Reindexing finished")
        if result != -1:
            condition_count = 1
            f.close()
            break
    
    # check pork
    event_value = 0
    if condition_count == 1:
        f = open(plugin_output_files, "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find("Disconnect block")
            if result != -1:
                event_value = 1
                continue
            if event_value == 1:
                result = line.find("height=6")
                if result != -1:
                    print("Fail dump file load ... ")
                    print("There are problems about initial block hashes ... ")
                    f.close()
                    sys.exit(1)

        condition_count = 2
        f.close()


    if condition_count == 2:
        the_path = utils.get_datadirDumpfile_path(abs_path, difficulty)
        the_path = the_path + "/coinflip_hash.txt"
        f = open(the_path, "r")
        while True:
            line = f.readline()
            if not line: break
            ready_hash_list.append(line.strip())
        f.close()

        i = 0
        result_hash_list = []
        f = open(plugin_output_files, "r")
        while True:
            line = f.readline()
            if not line:break
            result = line.find("UpdateTip")
            if result != -1:
                if ready_hash_list[i] == line.split(" ")[3].split("=")[1]:
                    i += 1
                    pass
                # 0번째는 제네시스 블록이라 스킵.
                elif i == 0:
                    continue
                else:
                    print("Fail test dump file load ... ")
                    f.close()
                    sys.exit(1)
            if i == 7:
                print("Success test dump file load test ... ")
                f.close()
                break

    else:
        print("Fail load data file test ... (Not Reindexing finished)")
        f.close()
        sys.exit(1)



