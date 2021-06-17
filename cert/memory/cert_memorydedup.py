import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def get_nextIndex(line):
    for i in range(0,len(line)):
        result = line[i].find("pts")
        if result != -1:
            return i-1

def get_max_rss(target_path):
    i = 0 
    max_rss = 0
    f = open(target_path, "r")
    while True:
        line = f.readline()
        if not line: break
        try:
            rss_index = get_nextIndex(line.split(" "))
            rss_data = int(line.split(" ")[rss_index])
        except:
            continue

        if max_rss < rss_data:
            max_rss = rss_data      
    f.close()
    return max_rss

def check_output_logfile(target_path, rss_list):
    if os.path.isfile(target_path):
        rss_list.append(get_max_rss(target_path))
        pass
    else:
        print("Failed simulation ...")
        sys.exit(1)

if __name__ == '__main__':

    rss_list = []
    path = os.path.abspath("./")
    print("Emulation start...")
    exec_shell_cmd("rm -rf ps_results")

    print("\nStart dedup mode 1000Sec simulation ... ")
    target_xml = path + "/1000S.xml"
    datadir_path = "shadow_result/1000nodes_dedup"
    exec_shell_cmd("bash ps_run.sh 8 1000 1 %s %s > /dev/null 2>&1" %(target_xml, datadir_path))
    target_path = path + "/ps_results/dedup_1000s.log"
    check_output_logfile(target_path, rss_list)
    print("Successfully dedup mode 1000Sec simulation ...")

    print("\nStart non-dedup mode 1000Sec simulation ... ")
    datadir_path = "shadow_result/1000nodes_non_dedup"
    exec_shell_cmd("bash ps_run.sh 8 1000 2 %s %s > /dev/null 2>&1" %(target_xml, datadir_path))
    target_path = path + "/ps_results/non_dedup_1000s.log"
    check_output_logfile(target_path, rss_list)
    print("Successfully non-dedup mode 1000Sec simulation ...")

    print("\nStart dedup mode 2000Sec simulation ... ")
    target_xml = path + "/2000S.xml"
    datadir_path = "shadow_result/2000nodes_dedup"
    exec_shell_cmd("bash ps_run.sh 8 2000 1 %s %s > /dev/null 2>&1" %(target_xml, datadir_path))
    target_path = path + "/ps_results/dedup_2000s.log"
    check_output_logfile(target_path, rss_list)
    print("Successfully dedup mode 2000Sec simulation ...")

    print("\nStart non-dedup mode 2000Sec simulation ... ")
    datadir_path = "shadow_result/2000nodes_non_dedup"
    exec_shell_cmd("bash ps_run.sh 8 2000 2 %s %s > /dev/null 2>&1" %(target_xml, datadir_path))
    target_path = path + "/ps_results/non_dedup_2000s.log"
    check_output_logfile(target_path, rss_list)
    print("Successfully non-dedup mode 2000Sec simulation ...")

    delta_dedup = rss_list[2] - rss_list[0]
    delta_nondedup = rss_list[3] - rss_list[1]
    memory_dedup_rate = float(1 - (float(delta_dedup)/float(delta_nondedup))) * 100
    print("\nThe memory deduplication rate is --->  %0.2lf Percentage " %memory_dedup_rate)  
