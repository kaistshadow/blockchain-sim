import os
from subprocess import check_output
import argparse
import sys
import os
import lxml.etree as ET

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

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
    for i in range(0,node_count-1):
        path_command = "cd " + path + ";"
        the_command = path_command + "rm -rf data/bcdnode" + str(i)
        exec_shell_cmd(the_command)
        the_command = path_command + "mkdir -p data/bcdnode" + str(i)
        exec_shell_cmd(the_command)


def get_xmlfile(path):
    tx_condition_count = 0
    condition_count = 0
    while(1):
        if condition_count == 0:
            try: 
                sim_time = input("Input simulation time : ")    
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
            if (tx_condition_count == 1) | (tx_condition_count == 2):
                pass
            else:
                tx_mode = str(input("Input transaction injector (enable/disable) : "))
            if tx_mode == "enable":
                if tx_condition_count == 2:
                    pass
                else:
                    try:
                        tx_condition_count = 1
                        tx_sec = int(input("(To measure max tps, sec value is 0) Input transaction interval (sec) : "))
                    except ValueError as e:
                        print("Must input only number ! ")
                        continue

                try:
                    tx_condition_count = 2
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
                                   
                try:
                    if tx_condition_count == 3:
                        tx_cnt = int(input("input number of transcations ( -1 : infinite number ): "))
                    else:
                        continue

                except ValueError as e:
                    print("Must input only integer number !")
                    continue

                xml_command = "cd ..; python xmlGenerator.py" + " " + "1" + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty + " " + str(tx_cnt) +" " + str(tx_sec) + " " + str(number_bitcoins_transferred) + " " + path
                break

            elif tx_mode == "disable":
                xml_command = "cd ..; python xmlGenerator.py" + " " + "1" + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty + " " + path
                break
                
            else:
                print("Enter only one of them (enable/disable) ")
                continue

    exec_shell_cmd(xml_command)

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
        result = line.find('result":null,"error')
        if result != -1:
            f.close()
            print("Fail transaction test ...")
            sys.exit(1)
    f.close()
    print("Success transaction test ...")

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
    if tx_mode == "enable":
        set_plugin_file(len(node_id_list), path)
        remove_tx_plugin(target_folder_xml)
        target_folder_xml = target_folder_xml[:len(target_folder_xml)-4] + "2.xml"
        shadow_command = "shadow " + target_folder_xml
        return shadow_command

    if tx_mode == "disable":  
        shadow_command = "shadow output.xml"
        return shadow_command
