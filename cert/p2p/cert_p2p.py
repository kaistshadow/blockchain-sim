import os
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def check_node_count(xmlfile):
    count = 0
    f = open(xmlfile, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("node id")
        if result != -1:
            count += 1
    f.close()
    return count

def test_nodediscovery(target_output_file):
    standard_num = 0
    f = open(target_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("stored addr num:")
        if result != -1:
            standard_num = int(line.split(":")[1][:-1])
            f.close()
            break
    f.close()

    f = open(target_output_file, "r")
    for line in f.readlines()[::-1]:
        result = line.find("stored addr num:")
        if result != -1:
            target_num = int(line.split(":")[1][:-1])
            if target_num > standard_num:
                f.close()
                return True
    f.close()
    return False

def test_gossip(target_output_file, target_blockID, target_blockHash):
    _blockID = ""
    condition_value = 0
    f = open(target_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        if condition_value == 0:
            result = line.find("blockID")
            if result != -1:
                _blockID = line.split(":")[1][:-1]
                if _blockID == target_blockID:
                    condition_value = 1
        if condition_value == 1:
            result = line.find("blockhash")
            if result != -1:
                if target_blockHash == line.split(":")[1][:-1]:
                    f.close()
                    return True

    f.close() 
    print(target_output_file)
    return False

def get_compare_value(target_output):
    target_blockID = ""
    target_blockHash = ""

    conditionValue_blockID = 0
    conditionValue_blockHash = 0

    f = open(target_output, "r")
    while True:
        line = f.readline()
        if not line: break

        if conditionValue_blockID == 0:
            result = line.find("blockID:")
            if result != -1:
                target_blockID = line.split(":")[1][:-1]
                conditionValue_blockID = 1

        if conditionValue_blockHash == 0:
            result = line.find("blockhash:")
            if result != -1:
                target_blockHash = line.split(":")[1][:-1]
                conditionValue_blockHash = 1

        if (conditionValue_blockHash == 1) & (conditionValue_blockID == 1):
            break
    f.close()
    return target_blockID, target_blockHash

def check_p2p_abilities(outputfile_list, target_path, node_count):

    target_blockID = ""
    target_blockHash = ""
    nodediscovery_flag = []
    gossip_flag = []
    for i in range(0,len(outputfile_list)):
        gossip_flag.append(False)
        nodediscovery_flag.append(False)

    for i in range(0,len(outputfile_list)):
        target_output_path = target_path + "/" + outputfile_list[i]
        target_output_file = target_output_path + "/" + os.listdir(target_output_path)[0]
        if i == 0:
            target_blockID, target_blockHash = get_compare_value(target_output_file)
        gossip_flag[i] = test_gossip(target_output_file, target_blockID, target_blockHash)
        nodediscovery_flag[i] = test_nodediscovery(target_output_file)

    if all(gossip_flag):
        print("Gossip simulation result : Success ")
    else:
        print("Gossip simulation result : Fail")
    
    if any(nodediscovery_flag):
        print("NodeDiscovery simulation result : Success")
    else:
        print("NodeDiscovery simulation result : Fail")

if __name__ == '__main__':

    path = os.path.abspath("./")
    print("Start p2p 10nodes emulation ...")
    exec_shell_cmd("shadow -h 10000 -d shadow_result/10nodes -w 8 Test_p2p_10nodes.xml > /dev/null 2>&1")
    
    # output file check
    xmlpath = path + "/Test_p2p_10nodes.xml"
    node_count = check_node_count(xmlpath) - 1
    output_path = path + "/shadow_result/10nodes/hosts"
    if node_count == len(os.listdir(output_path)):
        print("Successfully 10nodes emulation ...")
    else:
        print("Failed 10nodes emulation ...")

    # p2p regression test
    output_file_list = os.listdir(output_path)
    check_p2p_abilities(output_file_list, output_path, node_count)
    
    # 1000 nodes simulation start
    print("-------------------------------------------------------------------------------------")
    print("Start p2p 1000nodes emulation ...")
    exec_shell_cmd("shadow -h 10000 -d shadow_result/1000nodes -w 8 Test_p2p_1000nodes.xml > /dev/null 2>&1")

    # output file check
    xmlpath = path + "/Test_p2p_1000nodes.xml"
    node_count = check_node_count(xmlpath) - 1
    output_path = path + "/shadow_result/1000nodes/hosts"
    if node_count == len(os.listdir(output_path)):
        print("Successfully 1000nodes emulation ...")
    else:
        print("Failed 1000nodes emulation ...")

    # p2p regression test
    output_file_list = os.listdir(output_path)
    check_p2p_abilities(output_file_list, output_path, node_count)

