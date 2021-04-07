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

# 시뮬레이션된 노드들의 ip address 가져오기
def get_address_list(xml_file):
    ip_list = []
    f = open(xml_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("iphint")
        if result != -1:
            IP_address = line.split("iphint")[1].split("=")[1].split(">")[0]
            ip_list.append(IP_address[1:len(IP_address)-1])
    f.close()
    return ip_list
        
# xml 파일에서 플러그인마다 peer connection을 위해 addnode로 명시된 ip 주소들을 리스트 형식으로 뽑아옴.
def get_addnode_list(IP_list, xml_file):
    add_node_list = []
    f = open(xml_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("addnode")
        if result != -1:
            split_list = line.split(" ")
            target_list = []
            for i in range(0,len(split_list)):
                result = split_list[i].find("addnode")
                if result != -1:
                    print(split_list[i])
                    mapping_ip = split_list[i].split("=")[1].split(":")[0]
                    target_list.append(mapping_ip)
                    
            add_node_list.append(target_list)        
        
    
    f.close()
    return add_node_list

# plugin-client output 파일에서 "getpeerinfo" rpc response 값에서 각 노드마다 peer 들을 list를 통해 가져오기. (노드 순서 == 리스트 인덱스 순서)
def get_peerinfo(simulation_output_file, IP_list):
    final_list = []
    for i in range(0,len(IP_list)):
        f = open(simulation_output_file[len(IP_list) + i], "r")
        result_list = []
        while True:
            line = f.readline()
            if not line: break
            result = line.find("addrlocal")
            if result != -1:
                for j in range(0,len(IP_list)):
                    if i == j:
                        continue
                    result = line.find(IP_list[j])
                    if result != -1:
                        result_list.append(IP_list[j])
                final_list.append(result_list)
        f.close()

    return final_list

# shadow 시뮬레이션 시간 출력.
def get_runtime_shadow(output_txt):
    run_time = ""
    f = open(output_txt, "r")
    for line in f.readlines()[::-1]:
        result = line.find("run time was")
        if result != -1:
            run_time = line.split(" ")[16]
            break
    f.close()
    return run_time

def get_datadirDumpfile_path(abs_path, difficulty):
    the_list = abs_path.split("/")
    the_command = ""
    for i in range(0,len(the_list)):
        the_command = the_command + the_list[i] + "/"
        if the_list[i] == "blockchain-sim":
            if difficulty == "1":
                the_command = the_command + "testlibs/dump/difficulty_1"
            elif difficulty == "2":
                the_command = the_command + "testlibs/dump/difficulty_2"
            elif difficulty == "3":
                the_command = the_command + "testlibs/dump/difficulty_3"
            else:
                print("Fail load dump file ...")
            
            break
    return the_command

def get_difficulty_fromXML(xmlfile):
    f = open(xmlfile, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("difficulty")
        if result != -1:
            line_result = line.split(" ")
            for i in range(0,len(line_result)):
                result = line_result[i].find("difficulty")
                if result != -1:
                    return_difficulty = line_result[i].split("=")[1]
                    break
    f.close()
    return return_difficulty[0]
    
def get_last_hashValue(shadow_output_file, rpc_output_file, node_count, pork_count):
    condition_count = 0
    i = 0
    for z in range(0,node_count):
        f = open(rpc_output_file[z+node_count] , "r")
        for line in f.readlines()[::-1]:
            line.rstrip()
            result = line.find("bestblockhash")
            if result != -1:
                line = line.split(",")[3].split(":")[1]
                genesisHash = line[1:len(line)-1]
                i += 1
                if i == (pork_count+1):
                    break
                else:
                    continue

        if os.path.isfile(shadow_output_file[z]):
            f = open(shadow_output_file[z], "r")
            while True:
                line = f.readline()
                if not line: break
                result = line.find(genesisHash)
                if result != -1:
                    condition_count += 1

    return condition_count

def filter_testlibs_path(target_path):
    the_path = ""
    split_path = target_path.split("/")
    for i in range(0,len(split_path)):
        if split_path[i] == "blockchain-sim":
            the_path += split_path[i]
            the_path += "/"
            break
        the_path += split_path[i]
        the_path += "/"

    the_path += "/testlibs"
    return the_path

def filter_block_hash(plugin_output_files, node_count):
    node_list = []
    for i in range(node_count):
        line = []
        node_list.append(line)

    for i in range(0,len(plugin_output_files)):
        result = plugin_output_files[i].find("stdout-monitor.BITCOIN_MONITOR.1000")
        if result != -1:
            f = open(plugin_output_files[i], "r")
            while True:
                line = f.readline()
                if not line: break
                result = line.find("[INV] MSGBLOCK : blockhash =")
                if result != -1:
                    split_result = line.split("=")
                    block_hash = split_result[1].split("tx")[0].strip()
                    from_node = split_result[3].strip()
                    node_list[int(from_node)-16].append(block_hash)

            f.close()
    
    return node_list

