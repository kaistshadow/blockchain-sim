import os
from subprocess import check_output
import argparse
import sys
import time

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

if __name__ == '__main__':

    print("Start emulation processing certification test ...")
    path = os.path.abspath("./")

    # Get playing node count and running time from xml file.
    xmlfile = path + "/emulation_processing.xml"
    running_time, node_count = get_infos_fromXML(xmlfile)

    print("Start POW blockchain simulation ... ")
    emulation_time = time.time()
    exec_shell_cmd("shadow -w 3 emulation_processing.xml > /dev/null 2>&1")
    emulation_time = time.time() - emulation_time
    print("Success POW blockchain simulation ...")

    result_value = node_count
    blockHash_list, result_value = get_blockhash(path, result_value)

    print("---------------------------------------------------")
    print("1. Real time : %d Sec" %emulation_time)
    print("2. Emulation time : %d Sec" %running_time )
    print("3. Running nodes : %d nodes" %node_count)
    print("4. Successful emulated nodes : %d" %result_value)
    print("---------------------------------------------------")
    if result_value == node_count:
        if int(emulation_time) < int(running_time):
            print("\t Certification test result : Success")
        else:
            print("\t Certification test result : Fail")
    else:
        print("\t Certification test result : Fail")

        

