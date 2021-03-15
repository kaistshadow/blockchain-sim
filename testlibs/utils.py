import os
from subprocess import check_output
import argparse
import sys
import os
import lxml.etree as ET
import subprocess
import math

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)


# xml 생성 시, injector 사용이 필요없는 테스트가 있음. 이럴 경우 xml 파일에 transcation.so 파일에 
# 대한 정의가 되어있으면 안됨. 하지만 example.xml에는 되어 있기에, 이 함수를 통해 그에 대한 정의를 삭제를 해주고, 수정된 xml 파일을 return함
def remove_tx_plugin(tx_plugin):
    target_length = len(tx_plugin)
    update_file = tx_plugin[:target_length-4] + "2.xml" 
    fr = open(tx_plugin, "r")
    fw = open(update_file, "w")
    while True:
        line = fr.readline()
        if not line: break
        result = line.find("transaction.so")
        if result != -1:
            continue
        fw.write(line)
    fw.close()
    fr.close()

def renew_xml(tx_mode, target_folder_xml):
    print(tx_mode)
    if tx_mode == "disable":
        remove_tx_plugin(target_folder_xml)
        target_folder_xml = target_folder_xml[:len(target_folder_xml)-4] + "2.xml"
        shadow_command = "shadow " + target_folder_xml
        return shadow_command

    if tx_mode == "enable":  
        shadow_command = "shadow output.xml"
        return shadow_command

def set_plugin_file(node_count, path):
    if(os. path. isdir(path)):
        the_command = "rm -rf data/*"
        exec_shell_cmd(the_command)

    for i in range(0,node_count):
        the_command = "mkdir -p data/bcdnode" + str(i)
        exec_shell_cmd(the_command)

# Description : xml 파일을 생성하기 위해, 현재 실행되는 함수의 경로에서 xml파일을 생성하기 위한 command를 생성하여 반환함.
# input param : 현재 경로
# output return value : emulation_mode, path_command
def path_filter(path):
    the_path_command = ""
    path_list = path.split("/")
    path_abs = path_list[len(path_list)-2]
    if path_abs == "BLEEPemul":
        the_path_command = "cd ../../testlibs"
        return 1, the_path_command
    elif path_abs == "1_bitcoin":
        the_path_command = "cd ../../../../testlibs"
        return 0, the_path_command
    elif path_abs == "2_sybilAPI":
        the_path_command = "cd ../../../../testlibs"
        return 0, the_path_command
    else:
        print("Fail setting path ... ")
        sys.exit(1)


def subprocess_open(command):
    popen = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    (stdoutdata, stderrdata) = popen.communicate()
    return stdoutdata, stderrdata

# make shadow runtime format example of 00:00:09
def get_time_form(runtime):
    result = ""
    hours, mins, sec = 0, 0, 0
    target_time = int(runtime) - 1
    if target_time > 3600:
        hours = math.trunc(target_time/3600)
        mins = math.trunc((target_time%3600)/60)
        sec = math.trunc((target_time%3600)%60)
    elif target_time < 3600:
        mins = math.trunc(math.trunc((target_time%3600)/60))
        sec = math.trunc((target_time%3600)%60)
    else:
        hours = 1
        sec = sec - 1

    if len(str(hours)) == 2:
        result = result + str(hours) + ":"
    else:
        result = "0" + str(hours) + ":"
    if len(str(mins)) == 2:
        result = result + str(mins) + ":"
    else:
        result = result + "0" + str(mins) + ":"
    if len(str(sec)) == 2:
        result = result + str(sec)
    else:
        result = result + "0" + str(sec)

    return result


# xml parsing error : 플러그인 파일이 없을 때 발생하는 에러로서, shadow error 필터링 기준 중 하나임.
def filter_fail_shadow_test(output_file):
    f = open(output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find('error')
        if result != -1:
            result = line.find('Shadow XML parsing error')
            if result != -1:
                result_split = line.split(":")[6]
                print("----------------------Error content------------------------\n")
                print(result_split)
                print("--------------------------------------------------------------")
                break
    f.close()
