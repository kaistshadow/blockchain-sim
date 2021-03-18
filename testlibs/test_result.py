#
# 2021-03-15
# created by hong joon
#
import os
import subprocess
import argparse
import sys
import time
from testlibs import test_modules, utils

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def get_blockHash_list(node_output_file):
    block_hash_list = []
    f = open(node_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("UpdateTip")
        if result != -1:
            block_soc = []
            block_hash = line.split(" ")[3].split("=")[1]
            block_soc.append(block_hash)
            block_height = line.split(" ")[4].split("=")[1]
            block_soc.append(block_height)
            block_hash_list.append(block_soc)
                        
    f.close()
    return block_hash_list


def filter_overlap_height(node_output_file):
    condition_var = False
    result_list = []
    block_hash_list = get_blockHash_list(node_output_file)
    for i in range(0,len(block_hash_list)):
        result_list.append(block_hash_list[i])
        if i == 0:
            continue
        for j in range(0,len(result_list)-1):
            if block_hash_list[i][1] in result_list[j][1]:
                result_list[j][0] = block_hash_list[i][0]
                result_list.pop()
    return result_list


def summary_result(node_list, node_output_file, sim_time):

    last_blockHash_list = []
    block_count_list = []
    
    for z in range(0,len(node_list)):
        block_hash_list = filter_overlap_height(node_output_file[z])
        path = "./shadow.data/result_"+time.strftime('%H:%M:%S')+"_%d.log"%z
        f = open(path, "w")
        txs = test_modules.test_transaction_count(node_output_file)
        f.write("---------------------------------------------------------------------------------\n")
        f.write("1. node 갯수 : %d\n" %len(node_list))
        f.write("2. simulation time : %s sec\n" %sim_time)
        f.write("3.  실행된 노드 IP list\n")
        for i in range(0,len(node_list)):
            f.write("\t3-%d IP address : %s\n" %(i+1, node_list[i]))
        f.write("4. 생성된 블록 개수 : %d\n" %len(block_hash_list))
        f.write("5. 마지막 블록의 hash 값 : %s\n" %block_hash_list[len(block_hash_list)-1][0])
        last_blockHash_list.append(block_hash_list[len(block_hash_list)-1][0])
        f.write("6. 생성된 트랜잭션 개수 : %d\n" %txs)
        f.write("7. TPS : %s\n" %(txs/int(sim_time)))
        f.write("---------------------------------------------------------------------------------\n")
        f.write("8. Blockhash list\n")
        for i in range(0,len(block_hash_list)):
            f.write("\t8-%d blockhash : %s\n" %((i+1), block_hash_list[i][0]))
        f.close()
    
    path = "./shadow.data/peer_connection_result.txt"
    f = open(path, "w")
    filter_duplicate_value = set(last_blockHash_list)
    last_blockHash_list = list(filter_duplicate_value)
    filter_duplicate_value = set(block_count_list)
    block_count_list = list(filter_duplicate_value)
    # n개의 노드의 output 데이터들 담은 list에 중복 제거를 통해 list에 담긴 값이 1개면, 동기화가 잘 이루어졌다는 상황.
    if 1 == len(last_blockHash_list):
        f.write("Last block hash match rate : %d/%d\n" %(len(node_list),len(node_list)))
    else:
        f.write("Last block hash match rate : %d/%d\n" %(len(last_blockHash_list), len(node_list)))

    IP_list = utils.get_address_list("output.xml")
    # peer_connection_result 리스트에는 각 peer들의 "getpeerinfo" rpc response 로그의 "ipaddr" 속성값들을 가져옴.
    # 예를들어 1.0.0.1의 노드는 ['1.1.0.1', '1.2.0.1'] 리스트를 가져와서 리스트가 곧 속성 값으로 저장이되는 식으로 2중 리스트 형식으로 구현됨.
    peer_connection_result = utils.get_peerinfo(node_output_file, IP_list)

    f.write("peer connection status : \n\n")
    for i in range(0,len(peer_connection_result)):
        f.write("\t node : %s  ---------- connection match rate : %d/%d \n" %(IP_list[i], len(peer_connection_result[i]), len(IP_list)-1))
        for j in range(0,len(peer_connection_result[i])):
            f.write("\t\t\t - %s \n" %peer_connection_result[i][j])
    f.close()

