#
# 2021-03-15
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

# Description : blockchain plugin의 data dir를 설정해줘야할 디렉토리로서, 플러그인 개수만큼 제거하고 생성을 해줌.
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

# Description : shadow output.txt 를 만들기 위한 함수.
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

# bitcoin 실행 로그로 부터, port, rpc port, datadir 정보를 가져옴.
def get_plugin_args(plugin_infos):
    result_list = []
    f = open(plugin_infos, "r")
    while True:
        line = f.readline()
        if not line:break
        result = line.find('plugin="bitcoind"')
        if result != -1:
            split_list = line.split("arguments=")
            break
    
    split_list = split_list[1].split(" ")
    for i in range(0,len(split_list)):
        result = split_list[i].find("port")
        if result != -1:
            result_list.append(split_list[i].split("=")[1])
        result = split_list[i].find("datadir")
        if result != -1:
            result_list.append(split_list[i].split("=")[1])

    return result_list

# bitcoin log로 부터 difficulty 정보를 가져옴.
def get_difficulty_info(xml_file):
    count = 0
    f = open(xml_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("difficulty")
        if result != -1:
            split_list = line.split(' ')
            for i in range(0,len(split_list)):
                result = split_list[i].find("difficulty")
                if result != -1:
                    for i in range(0,len(split_list)):
                        result = split_list[i].find("difficulty")
                        if result != -1:
                            difficulty = split_list[i].split("=")[1]
                            break
                    break
        if count != 0:
            break
    f.close()
    return difficulty[0]