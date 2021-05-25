#
# 2021-05-25
# created by hong joon
#

# test 스크립트 작성을 위해 필요한 함수들을 구현해 놓음.
import os
import sys
import os
import subprocess
from testlibs import bleep_regtest_utils

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def get_txgenerator_node(target_xml):
    f = open(target_xml, "r")
    iterf = iter(f)
    target_ip = ""
    condition_count = 0
    for line in iterf:
        if "iphint" in line:
            target_ip = line.split("=")[1][:-8]
            target_ip = target_ip[1:]
            condition_count = 1
            continue

        if condition_count == 1:
            
            if "txgenstartat=0" in line:
                condition_count = 2
                break
            
            if "txgenstartat=10000000" in line:
                condition_count = 0
                pass
            
    f.close()
    if condition_count == 2:
        return target_ip
    else:
        print("Fail tx generator setting ... ")
        sys.exit(1)
