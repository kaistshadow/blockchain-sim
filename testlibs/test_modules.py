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

# xml existence test - If there is no file in the path, return fail
def test_xml_existence(output):
    path = os.path.abspath(".")
    target_folder = path + "/" + output
    if os.path.isfile(target_folder):
        print("Success xml existence test ...")
        return target_folder
    else:
        print("Fail xml existence test ...")
        sys.exit(1)

# Get node_id, kill time, plugins in xml file
def get_xml_info_new(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    plugin_list=[]
    node_id_list =[]
    kill = root.findall('kill')
    runtime = kill[0].attrib['time']

    nodes =  root.findall('node')
    for node in nodes:
        node_id_list.append(node.attrib['id'])
        applications = node.findall('application')
        for a in applications:
            plugin_list.append(a.attrib['plugin'])

    return runtime, node_id_list, plugin_list

def set_plugin_file(node_count, path):
    if(os. path. isdir(path)):
        the_command = "rm -rf data/*"
        exec_shell_cmd(the_command)

    for i in range(0,node_count):
        the_command = "mkdir -p data/bcdnode" + str(i)
        exec_shell_cmd(the_command)

def get_xmlfile(path):
    emulation_start, path_abs = path_filter(path)
    the_command = path_abs + "; python xmlGenerator.py"
    tx_condition_count = 0
    condition_count = 0
    tx_injector_file = 0
    tx_so_file = path + "/transaction.so"
    xml_command = ""
    if emulation_start == 1:
        tx_injector_file = 3
    else: 
        if os.path.isfile(tx_so_file) == False:
            tx_injector_file = 1
        else:
            tx_injector_file = 2

    while(1):
        if condition_count == 0:
            try: 
                sim_time = input("Input simulation time (sec) : ")    
                if int(sim_time) > 0:
                    condition_count = 1
                else:
                    print("simulation time not negative ... ")
                    continue
            except ValueError as e:
                print("input only integer ...")
                continue

        if condition_count == 1:
            algo = str(input("Input mining algorithm(pow/coinflip) : "))
            if (algo == "pow") | (algo == "coinflip"):
                condition_count = 2
            else:
                print("Enter only one of them (pow/coninflip) ")
                continue

        if condition_count == 2:
            difficulty = str(input("Input difficulty(1/2/3) : "))
            if (difficulty == "1") | (difficulty == "2") | (difficulty == "3"):
                condition_count = 3
            else:
                print("Enter only one of them(1/2/3)")
                continue

        if condition_count == 3:
            if tx_condition_count == 0:
                tx_mode = str(input("Input transaction injector (enable/disable) : "))
                if tx_mode == "enable":
                    if tx_injector_file == 1:
                        print("Sorry there is no transaction.so file. so you must choose disable ... ")
                        continue
                    else:
                        tx_condition_count = 1               
               
                elif tx_mode == "disable":
                    if tx_injector_file == 2:
                        print("sorry This test is tranasction test so you must set tx_mode = enable ... ")
                        continue
                    else:
                        tx_condition_count = 1
                    xml_command = the_command + " " + "1" + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty + " " + path
                    break
                    
                else:
                    print("Enter only one of them (enable/disable) ")
                    continue

            elif tx_condition_count == 1:

                try:
                    tx_cnt = int(input("input number of transcations ( -1 : infinite number ): "))
                    if tx_cnt > 0:
                        tx_condition_count = 2

                    elif tx_cnt == -1:
                        number_bitcoins_transferred = 0.0000546
                        tx_sec = 0
                        xml_command = the_command + " " + "1" + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty + " " + str(tx_cnt) +" " + str(tx_sec) + " " + str(number_bitcoins_transferred) + " " + path
                        break                            

                    elif tx_cnt < 0:
                        print("Must input only integer number !")
                        continue
                    
                except ValueError as e:
                    print("Must input only integer number !")
                    continue
            
            elif tx_condition_count == 2:

                try:
                    number_bitcoins_transferred = float(input("input number of Bitcoins transferred (minimum amount : 0.0000546) : "))
                    if number_bitcoins_transferred < 0.0000546:
                        print("The minimum transfer fee is '0.0000546' bitcoin ...")
                        continue
                    else:
                        if str(number_bitcoins_transferred).split(".")[1] == "0":
                            number_bitcoins_transferred = int(number_bitcoins_transferred)
                        tx_condition_count = 3
                except ValueError as e:
                    print("Must input only number !")
                    continue

            elif tx_condition_count == 3:
               
                try:
                    tx_sec = int(input("Input transaction interval (sec) : "))
                    if tx_sec > 0:
                        xml_command = the_command + " " + "1" + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty + " " + str(tx_cnt) +" " + str(tx_sec) + " " + str(number_bitcoins_transferred) + " " + path
                        break
                    else:
                        print("Must input only number ! ")
                        continue
                except ValueError as e:
                    print("Must input only number ! ")
                    continue


    exec_shell_cmd(xml_command)
    return tx_mode

# After shadow, check output data
def test_file_existence(node_id_list, plugin_list):
    if len(node_id_list) != len(plugin_list):
        sys.exit(1)
    path = os.path.abspath(".")
    target_folder_list = []
    for i in range(0,len(node_id_list)):
        target_path = path + "/shadow.data/hosts/" + node_id_list[i] + "/stdout-" + node_id_list[i] + "." + plugin_list[i] + ".1000.log"
        target_folder_list.append(target_path)
    for i in range(0,len(target_folder_list)):
        if os.path.isfile(target_folder_list[i]) == False:
            print("Fail not existence file - %s" %(target_folder_list[i]))
            sys.exit(1)
    print("Success blockchain test output file existence ...")
    return target_folder_list

# If rpc output file has error log, the transaction is not created properlys
def test_transaction_existence(simulation_output_file):
    f = open(simulation_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find('"error":null')
        if result != -1:
            f.close()
            print("Success transaction test ...")
            sys.exit(0)
    f.close()
    print("Fail transaction test ...")
    sys.exit(1)
    
def test_shadow_output_file_existence():
    path = os.path.abspath(".")
    target_folder_file = path + "/output.txt"
    if os.path.isfile(target_folder_file):
        return target_folder_file
    else:
        print("Fail not existence shadow output file ... ")
        sys.exit(1)

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

# test1 : whether runtime setting worked or not 
# test2 : whether plugin(node_id) worked or not
def test_shadow(output_file, runtime, node_id_list):
    f = open(output_file, "r")
    # result_count more than 3 means success.
    result_count = 0
    return_count = 0
    return_time = get_time_form(runtime)
    while True:
        line = f.readline()
        if not line: break
        result = line.find("_process_start")
        if result != -1:
            result = line.find("has set up the main pth thread")
            if result != -1:
                for i in range(0,len(node_id_list)):
                    result = line.find(node_id_list[i])
                    if result != -1:
                        result_count = 1
              
        result = line.find(return_time)
        if result != -1:
            if result_count == 1:
                f.close()
                print("Success shadow test ...")
                return_count = 1
                break
            else:
                f.close()
                print("Fail shadow test] - runtime error ...")
                sys.exit(1)
    if return_count == 0:            
        f.close()
        print("[Fail shadow test] - plugin does not run ... (check the logs)")
        sys.exit(1)
    else:
        pass

def subprocess_open(command):
    popen = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE, shell=True)
    (stdoutdata, stderrdata) = popen.communicate()
    return stdoutdata, stderrdata
    