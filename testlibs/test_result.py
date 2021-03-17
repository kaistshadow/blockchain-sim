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

def count_block(node_output_file):
    block_count = 0
    f = open(node_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("UpdateTip")
        if result != -1:
            block_count += 1
    f.close()
    return block_count

def get_last_blockHash(node_output_file):
    f = open(node_output_file, "r")
    for line in f.readlines()[::-1]:
        result = line.find("UpdateTip")
        if result != -1:
            split_data = line.split(" ")[3].split("=")[1]
            break
    f.close()
    return split_data

def get_blockHash_list(node_output_file):
    block_hash_list = []
    f = open(node_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("UpdateTip")
        if result != -1:
            block_hash_list.append(line.split(" ")[3].split("=")[1])
                        
    f.close()
    set_list = set(block_hash_list)
    block_hash_list = list(set_list)
    return block_hash_list

def summary_result(node_list, node_output_file, sim_time):

    last_blockHash_list = []
    block_count_list = []

    for z in range(0,len(node_list)):
        path = "./shadow.data/result_"+time.strftime('%H:%M:%S')+"_%d.log"%z
        f = open(path, "w")
        txs = test_modules.test_transaction_count(node_output_file)
        f.write("---------------------------------------------------------------------------------\n")
        f.write("1. node 갯수 : %d\n" %len(node_list))
        f.write("2. simulation time : %s sec\n" %sim_time)
        for i in range(0,len(node_list)):
            f.write("3.  ")
            f.write("\t3-%d IP address : %s\n" %(i+1, node_list[i]))
        block_count = count_block(node_output_file[z])
        block_count_list.append(block_count)
        f.write("4. 생성된 블록 개수 : %d\n" %block_count)
        f.write("5. 마지막 블록의 hash 값 : %s\n" %get_last_blockHash(node_output_file[z]))
        last_blockHash_list.append(get_last_blockHash(node_output_file[z]))
        f.write("6. 생성된 트랜잭션 개수 : %d\n" %txs)
        f.write("7. TPS : %s\n" %(txs/int(sim_time)))
        filter_duplicate_value = set(last_blockHash_list)
        last_blockHash_list = list(filter_duplicate_value)
        filter_duplicate_value = set(block_count_list)
        block_count_list = list(filter_duplicate_value)
        # n개의 노드의 output 데이터들 담은 list에 중복 제거를 통해 list에 담긴 값이 1개면, 동기화가 잘 이루어졌다는 상황.
        if 1 == len(last_blockHash_list):
            f.write("8. Last block hash match rate : %d/%d\n" %(len(node_list),len(node_list)))
        else:
            f.write("8. Last block hash match rate : %d/%d\n" %(len(last_blockHash_list), len(node_list)))

        f.write("---------------------------------------------------------------------------------\n")
        f.write("9. Blockhash list\n")
        block_hash_list = get_blockHash_list(node_output_file[z])
        for i in range(0,len(block_hash_list)):
            f.write("\t9-%d blockhash : %s\n" %((i+1), block_hash_list[i]))
        f.close()
    
    path = "./shadow.data/peer_connection_result.txt"
    f = open(path, "w")
    IP_list = utils.get_address_list("output.xml")
    peer_connection_result = test_modules.FF01_test_peerConnection(node_output_file, IP_list)

    f.write("peer connection status : \n\n")
    for i in range(0,len(peer_connection_result)):
        f.write("\t node : %s  ---------- connection match rate : %d/%d \n" %(IP_list[i], len(peer_connection_result[i]), len(IP_list)-1))
        for j in range(0,len(peer_connection_result[i])):
            f.write("\t\t\t - %s \n" %peer_connection_result[i][j])
    f.close()

