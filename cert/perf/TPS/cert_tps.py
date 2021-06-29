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
        
    if condition_count == len(target_list) - 1:
        print("Succesfully node setting ...")
    else:
        print(condition_count, len(target_list))
        print("Failed node setting ...")
        sys.exit(1) 

if __name__ == '__main__':

    start = time.time()
    exec_shell_cmd("rm -rf shadow.data")
    print("Emulation start(cert TPS) ...")
    exec_shell_cmd("shadow test-BLEEPLib-TPS.xml > /dev/null 2>&1")

    path = os.path.abspath("./")
    target_path = path + "/shadow.data/hosts"
    target_directory_list = os.listdir(target_path)
    check_nodeId(target_path, target_directory_list)

    tx_cnt = 0
    total_tps = 0
    max_tps = 0
    min_tps = 100
    average_tps = 0
    tps_cnt = 0

    target_log = path + "/shadow.data/hosts/txgenerator/stdout-txgenerator.TESTER.1000.log"
    f = open(target_log, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("TPS=")
        if result != -1:
            target_tps = float(line.split("=")[2].split("/")[0][0:3])
            tx_cnt = float(line.split("/")[1].split("=")[1])
            total_tps += target_tps
            tps_cnt += 1

            if max_tps < target_tps:
                max_tps = target_tps
            elif min_tps > target_tps:
                min_tps = target_tps
    f.close()

    print("--------------------------------------------------------------------------------------------------")
    print("\t\t\t\t Total transaction : %d Count" %tx_cnt)
    print("\t\t\t\t Max TPS : %0.2lfS" %max_tps)
    print("\t\t\t\t Min TPS : %0.2lfS" %min_tps)
    print("\t\t\t\t Average TPS : %0.2lfS" %(total_tps/tps_cnt))
    print("--------------------------------------------------------------------------------------------------")
    runtime = time.time() - start
    print("\t\t\t\tRunTime : %d Sec" %runtime)
