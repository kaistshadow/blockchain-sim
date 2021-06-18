import os
import argparse
import sys
import time
import random

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

def test_gossip(target_output_file, target_blockID, target_blockHash, target_blockNumber):
    _blockID = ""
    condition_value = 0
    f = open(target_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        target_text = "blockID:" + str(target_blockNumber)
        result = line.find(target_text)
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

def get_compare_value(target_output, nodes):
    target_blockID = ""
    target_blockHash = ""
    target_blockNumber = 0

    if nodes == 10:
        target_blockNumber = random.randrange(2,10)
    if nodes == 1000:
        target_blockNumber = random.randrange(2,50)
    
    conditionValue_blockID = 0
    conditionValue_blockHash = 0
    condition_count = 0

    f = open(target_output, "r")
    while True:
        line = f.readline()
        if not line: break

        if conditionValue_blockID == 0:
            target_text = "blockID:" + str(target_blockNumber)
            result = line.find(target_text)
            if result != -1:
                target_blockID = line.split(":")[1][:-1]
                conditionValue_blockID = 1
                condition_count = 1

        if conditionValue_blockHash == 0:
            if condition_count == 1:
                result = line.find("blockhash:")
                if result != -1:
                    target_blockHash = line.split(":")[1][:-1]
                    conditionValue_blockHash = 1

        if (conditionValue_blockHash == 1) & (conditionValue_blockID == 1):
            break
    f.close()
    return target_blockID, target_blockHash, target_blockNumber

def check_p2p_abilities(outputfile_list, target_path, node_count):

    count = 0
    target_blockID = ""
    target_blockHash = ""
    nodediscovery_flag = []
    gossip_flag = []
    target_blockNumber = 0

    for i in range(0,len(outputfile_list)):
        gossip_flag.append(False)
        nodediscovery_flag.append(False)

    for i in range(0,len(outputfile_list)):
        target_output_path = target_path + "/" + outputfile_list[i]
        target_output_file = target_output_path + "/" + os.listdir(target_output_path)[0]
        if i == 0:
            target_blockID, target_blockHash, target_blockNumber = get_compare_value(target_output_file, node_count)
        gossip_flag[i] = test_gossip(target_output_file, target_blockID, target_blockHash, target_blockNumber)
        nodediscovery_flag[i] = test_nodediscovery(target_output_file)
        if nodediscovery_flag[i] == True:
            count += 1

    if all(gossip_flag):
        print("Gossip simulation result : Success ")
    else:
        print("Gossip simulation result : Fail")
    
    if int(len(outputfile_list)/2) <= count:
        print("NodeDiscovery simulation result : Success")
        print("NodeDiscovery test target blockID : %d" %target_blockNumber)
    else:
        print("NodeDiscovery simulation result : Fail")

def p2p_test(test_count, path):
    result_condition_count = 0
    print("------------------------------------------------------")
    print("\t\t Start %d test" %(test_count+1))
    print("------------------------------------------------------")
    print("Start p2p 10nodes emulation ...")
    output_path = "shadow_result/10nodes" + "_" + str(test_count)
    exec_shell_cmd("shadow -h 10000 -d %s -w 8 Test_p2p_10nodes.xml > /dev/null 2>&1" %output_path)
    xmlpath = path + "/Test_p2p_10nodes.xml"
    node_count = check_node_count(xmlpath) - 1
    output_path = output_path + "/hosts"
    if node_count == len(os.listdir(output_path)):
        print("Successfully 10nodes emulation ...")
        result_condition_count += 1
    else:
        print("Failed 10nodes emulation ...")
    output_file_list = os.listdir(output_path)
    check_p2p_abilities(output_file_list, output_path, node_count)
    print("\nStart p2p 1000nodes emulation ...")
    output_path = "shadow_result/1000nodes" + "_" +str(test_count)
    exec_shell_cmd("shadow -h 10000 -d %s -w 8 Test_p2p_1000nodes.xml > /dev/null 2>&1" %output_path)
    xmlpath = path + "/Test_p2p_1000nodes.xml"
    node_count = check_node_count(xmlpath) - 1
    output_path = output_path + "/hosts"
    if node_count == len(os.listdir(output_path)):
        print("Successfully 1000nodes emulation ...")
        result_condition_count += 1
    else:
        print("Failed 1000nodes emulation ...")
    output_file_list = os.listdir(output_path)
    check_p2p_abilities(output_file_list, output_path, node_count)
    if result_condition_count == 2:
        return True
    else:
        return False

if __name__ == '__main__':

    try:
        test_count = int(sys.argv[1])
    except:
        print("Please Input test count")
        sys.exit(1)

    result_condition_count = 0
    start = time.time()
    path = os.path.abspath("./")
    result_flags = []

    for i in range(0, test_count):
        result_flags.append(False)

    for i in range(0, test_count):
        result_flags[i] = p2p_test(i, path)
    Runtime = time.time() - start
    if all(result_flags):
        print("------------------------------------------------------")
        print("\n\t\t Test result : Success")
    else:
        print("\t\t Test result : Fail")
    Runtime = time.time() - start
    print("\t\t Runtime : %d Sec" %Runtime)