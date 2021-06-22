import os
import argparse
import sys
import time

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)
    
def check_nodeId(target_path, target_list):
    condition_count = 0
    for i in range(0,len(target_list)):
        target_node = target_path + "/" + target_list[i]
        target_node_output_file = target_node + "/" + os.listdir(target_node)[0]

        f = open(target_node_output_file, "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find("myId:")
            if result != -1:
                target_id = line.split(":")[1][:-1]
                if target_id == target_list[i]:
                    condition_count += 1
        f.close()
        
    if condition_count == len(target_list)-1:
        print("Succesfully node setting ...")
    else:
        print("Failed node setting ...")
        sys.exit(1)

if __name__ == '__main__':

    start = time.time()
    exec_shell_cmd("rm -rf shadow.data")
    print("Emulation start(cert Latency) ... ")
    exec_shell_cmd("shadow test-BLEEPLib-Latency.xml > /dev/null 2>&1")

    path = os.path.abspath("./")
    target_path = path + "/shadow.data/hosts"
    target_directory_list = os.listdir(target_path)
    check_nodeId(target_path, target_directory_list)

    max_latency = 0
    min_latency = 100
    latency_cnt = 0
    total_latency = 0

    target_log = path + "/shadow.data/hosts/txgenerator/stdout-txgenerator.TESTER.1000.log"
    f = open(target_log, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("Latency=")
        if result != -1:
            target_latency = int(line.split("=")[1].split("s")[0])
            total_latency += target_latency
            latency_cnt += 1
            if max_latency < target_latency:
                max_latency = target_latency
            elif min_latency > target_latency:
                min_latency = target_latency
            

    f.close()
    print("--------------------------------------------------------------------------------------------------")
    print("\t\t\t\t Max Latency : %dS" %max_latency)
    print("\t\t\t\t Min Latency : %dS" %min_latency)
    print("\t\t\t\t Average Latency : %0.2lfS" %(total_latency/latency_cnt))
    print("--------------------------------------------------------------------------------------------------")
    runtime = time.time() - start
    print("\t\t\t\tSimulated Time : 300 Sec")
