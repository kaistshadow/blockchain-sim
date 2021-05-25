#
# 2021-05-25
# created by hong joon
#

# test 스크립트 작성을 위해 필요한 함수들을 구현해 놓음.
import os
from subprocess import check_output
import argparse
import sys
import os
import lxml.etree as ET
import subprocess
import math
from testlibs import bleep_regtest_utils

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def test_difficulty(simulation_output_file):
    eachNode_difficulty_list = []
    for i in range(0,len(simulation_output_file)):
        f = open(simulation_output_file[i], "rb")
        iterf = iter(f)
        for line in iterf:
            line = str(line)
            if "genesis blk generated and its hash=" in line:
                difficulty_hash = line.split("=")[1][:-3]
                eachNode_difficulty_list.append(difficulty_hash)
                break
        f.close()
    eachNode_difficulty_list = set(eachNode_difficulty_list)
    if 1 == len(eachNode_difficulty_list):
        print("Successfully blockchain difficulty test ... ")
        sys.exit(0)
    else:
        print("Failed blockchain difficulty test ... ")
        sys.exit(1)

def test_connection(simulation_output_file, IP_list):
    check_flags = []
    for i in range(0,len(simulation_output_file)):
        check_flags.append(False)

    for i in range(0,len(simulation_output_file)):
        f = open(simulation_output_file[i], "rb")
        iterf = iter(f)
        for line in iterf:
            line = str(line)
            if IP_list[len(IP_list)-1-i] in line:
                check_flags[i] = True
        f.close()

    if all(check_flags):
        print("Successfully peer connection test ...")
        sys.exit(0)
    else:
        print("Failed peer connection test ...")
        sys.exit(1)

def transaction_test(simulation_output_file, target_ip):
    check_flags = False
    for i in range(0,len(simulation_output_file)):
        if target_ip in simulation_output_file[i]:
            f = open(simulation_output_file[i], "rb")
            iterf = iter(f)
            for line in iterf:
                line = str(line)
                if "txgentimer generate new tx" in line:
                    check_flags = True
                    break
            f.close()
    if check_flags == True:
        print("Success transaction test ...")
        sys.exit(0)
    else:
        print("Fail transaction test ...")
        sys.exit(1)