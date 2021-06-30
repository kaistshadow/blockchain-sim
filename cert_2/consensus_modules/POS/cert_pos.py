import os
from subprocess import check_output
import argparse
import sys
import time
import copy

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def get_target_blockhash(target_path_file):
    condition_count = 6
    blockhash = ""
    f = open(target_path_file, "r")
    for line in f.readlines()[::-1]:
        result = line.find("blockhash:")
        if result != -1:
            if blockhash == line.split(":")[1][:-1]:
                continue
            else:
                if condition_count == 0:
                    f.close()
                    return blockhash
                else:
                    blockhash = line.split(":")[1][:-1]
                    condition_count -= 1
    f.close()

def get_lastSix_blockhash(target_path_file, input_target_blockhash):
    condition_count = 0
    target_blockHash = ""
    target_blockid = 0

    f = open(target_path_file, "r")
    for line in f.readlines()[::-1]:       
        result = line.find(input_target_blockhash)
        if result != -1:
            f.close()
            return input_target_blockhash

    f.close()
    return "Fail"

def get_blockhash(path, result_value):
    blockHash_list = []
    target_path = path + "/shadow.data/hosts" 
    node_outputfile_list = os.listdir(target_path)
    
    target_path_file = target_path + "/" + node_outputfile_list[0] 
    target_path_file = target_path_file + "/" + os.listdir(target_path_file)[0]        
    target_blockhash = get_target_blockhash(target_path_file)

    for i in range(0,len(node_outputfile_list)):
        target_path_file = target_path + "/" + node_outputfile_list[i] 
        target_path_file = target_path_file + "/" + os.listdir(target_path_file)[0]        
        target_value = get_lastSix_blockhash(target_path_file, target_blockhash)
        if len(target_value) > 4:
            blockHash_list.append(target_value)
        else:
            result_value -= 1

    return blockHash_list, result_value

def get_infos_fromXML(xmlfile):
    node_count = 0
    running_time = 0

    f = open(xmlfile, "r")
    while True:
        line = f.readline()
        if not line: break
        if running_time == 0:
            result = line.find("kill time")
            if result != -1:
                running_time = int(line.split('"')[1].split('"')[0])
        
        result = line.find("node id")
        if result != -1:
            node_count += 1
    f.close()
    node_count -= 1

    return running_time, node_count

def filter_rawblockHash(raw_blockHash):
    target_char = "[]"
    for i in range(0,len(target_char)):
        raw_blockHash = raw_blockHash.replace(target_char[i], "")
    return raw_blockHash

def get_blockCount(posnode_output_data):
    f = open(posnode_output_data, "r")
    for line in f.readlines()[::-1]:
        result = line.find("blockID")
        if result != -1:
            f.close()
            return int(line.split(":")[1][:-1])
    f.close() 
    return -1

# compare previous longest chain with present longest chain 
def compare_longestChain_hash(post_chain, present_chain):
    check_count = len(post_chain)
    for i in range(0,len(post_chain)):
        if post_chain[i] in present_chain:
            check_count -= 1
    if check_count == 0:
        return True
    else:
        return False
        
def compare_previousTime_with_presentTime(previous_time, present_time):
    previous_hour = int(previous_time.split(":")[0])
    previous_min =  int(previous_time.split(":")[1])
    previous_sec =  int(previous_time.split(":")[2])
    
    present_hour = int(present_time.split(":")[0])
    present_min = int(present_time.split(":")[1])
    present_sec = int(present_time.split(":")[2])
    
    previous_sec_time = (previous_hour * 60 * 60)+ (previous_min * 60) + previous_sec
    present_sec_time = (present_hour * 60 * 60)+ (present_min * 60) + present_sec

    if 540 >= (present_sec_time - previous_sec_time):
        return True
    else:
        return False

def check_node_liveness(posnode_output_data):
    previousTime = 0
    presentTime = 0
    liveness_condition_value = 0
    liveness_check_flags = []

    f = open(posnode_output_data, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("Gererated time:")
        if result != -1:
            previousTime = presentTime
            presentTime = line.split("/")[1][:-1]
            if liveness_condition_value != 0:
                liveness_check_flags.append(compare_previousTime_with_presentTime(previousTime, presentTime))
            liveness_condition_value = 1
    f.close()

    if all(liveness_check_flags):
        return True
    else:
        return False

def check_block_fork(posnode_output_data):
    condition_count = 0
    present_target_list = []
    post_target_list = []
    block_hash_list = []
    fork_rate_list = []

    previous_time = ""
    present_time = ""

    f = open(posnode_output_data, "r")
    while True:
        blank_list = []
        line = f.readline()
        if not line: break

        previous = present_time
        result = line.find("Gererated time:")
        if result != -1:
            present_time = line.split("/")[1][:-1]
        

        if condition_count == 0:
            result = line.find("======== LongestChain ========")
            if result != -1:
                post_target_list = present_target_list
                present_target_list = blank_list
                condition_count = 1
                continue

        if condition_count == 1:
            result = line.find("========================")
            if result != -1:
                fork_rate_list.append(compare_longestChain_hash(post_target_list, present_target_list))
                condition_count = 0
                continue
            present_target_list.append(filter_rawblockHash(line[:-1]))
            
    f.close()

    fork_count = fork_rate_list.count(False)
    return int(fork_count), len(fork_rate_list)

if __name__ == '__main__':

    print("Start POS consensus certification test ...")
    path = os.path.abspath("./")

    xmlfile = path + "/pos.xml"
    running_time, node_count = get_infos_fromXML(xmlfile)

    emulated_time = time.time()
    print("Start emulate POS consensus ... ")
    exec_shell_cmd("shadow pos.xml > /dev/null 2>&1")
    print("Finish emulate POS consensus ...")
    emulated_time = time.time() - emulated_time

    result_value = node_count
    blockHash_list, result_value = get_blockhash(path, result_value)
    safety_value = False

    if result_value == node_count:
        safety_value = True
    else:
        safety_value = False

    fork_count, blockCount = 0, 0
    block_count = 0
    liveness_result = False

    for i in range(0,node_count):
        target_path = path + "/" + "shadow.data/hosts/posnode%d/stdout-posnode%d.NODE.1000.log" %(i, i)
        if i == 0:
            block_count = get_blockCount(target_path)
            liveness_result = check_node_liveness(target_path)

        fork_count, blockCount = check_block_fork(target_path)
        if (fork_count/blockCount) == 0:
            continue
        else:
            break
    print("---------------------------------------------------------")
    print("\t\t Run time : %d" %emulated_time)
    print("\t\t Simulate time : %d" %running_time)
    print("\t\t Block count : %d" %block_count)
    print("\t\t Fork count : %d" %fork_count)
    # print("\t\t Fork rate : %f" %fork_rate)
    if safety_value == True:
        print("\t\t POS module safety result : Success")
    else:
        print("\t\t POS module safety result : Fail")
    
    if liveness_result == True:
        print("\t\t POS module liveness result : Success")
    else:
        print("\t\t POS module liveness result : Fail")
    print("---------------------------------------------------------")
    

