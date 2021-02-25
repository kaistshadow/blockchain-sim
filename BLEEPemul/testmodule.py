import os
import subprocess
import argparse
import sys
import math
import xml.etree.ElementTree as ET

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

# After shadow, check output data
def test_file_existence(node_id_list, plugin_list):
    if len(node_id_list) != len(plugin_list):
        sys.exit(1)
    path = os.path.abspath(".")
    target_folder_list = []
    for i in range(0,len(node_id_list)):
        target_path = path + "/datadir/hosts/" + node_id_list[i] + "/stdout-" + node_id_list[i] + "." + plugin_list[i] + ".1000.log"
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

# get xml file 
def get_xmlfile():
    tx_condition_count = 0
    condition_count = 0
    while(1):
        if condition_count == 0:
            sim_time = input("Input simulation time : ")    
            if int(sim_time) > 0:
                condition_count = 1
            else:
                print("simulation time not negative ... ")
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

                    if str(number_bitcoins_transferred).split(".")[1] == "0":
                        number_bitcoins_transferred = int(number_bitcoins_transferred)

                except ValueError as e:
                    print("Must input only number !")
                    continue
                xml_command = "python make_approximate_setmining_test.py" + " " + "1" + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty + " " + tx_cnt +" " + str(tx_sec) + " " + str(number_bitcoins_transferred)
                break

            elif tx_mode == "normal":
                xml_command = "python make_approximate_setmining_test.py" + " " + "1" + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty 
                break
            else:
                print("Enter only one of them (enable/disable) ")
                continue

    exec_shell_cmd(xml_command)
    xml_command = "mv base1N" + sim_time + "T.xml output.xml"
    exec_shell_cmd(xml_command)


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

# test1 : whether runtime setting worked or not 
# test2 : whether plugin(node_id) worked or not
def test_shadow(output_file, runtime, node_id_list):
    complete_node = []
    f = open(output_file, "r")
    # result_count more than 3 means success.
    result_count = 0
    return_time = get_time_form(runtime)
    while True:
        line = f.readline()
        if not line: break
        result = line.find("_process_start")
        if result != -1:
            result = line.find("has set up the main pth thread")
            if result != -1:
                result = line.find("bitcoind")
                if result != -1:
                    complete_node.append(line.split(" ")[4].split("~")[1][:-1])
                for i in range(0,len(node_id_list)):
                    result = line.find(node_id_list[i])
                    if result != -1:
                        result_count = 1
              
        result = line.find(return_time)
        if result != -1:
            if result_count == 1:
                f.close()
                print("Success shadow test ...")
                return complete_node, runtime
            else:
                f.close()
                print("Fail shadow test] - runtime error ...")
                sys.exit(1)
    f.close()
    print("[Fail shadow test] - plugin does not run ...")
    sys.exit(1)

def test_shadow_output_file_existence():
    path = os.path.abspath(".")
    target_folder_file = path + "/output.txt"
    if os.path.isfile(target_folder_file):
        return target_folder_file
    else:
        print("Fail not existence shadow output file ... ")
        sys.exit(1)

def test_transaction_count(simulation_output_file):
    txs_bitcoind = 0
    blocks_count = 0
    f = open(simulation_output_file[0], "r")
    for line in f.readlines()[::-1]:
        result = line.find("UpdateTip")
        if result != -1:
            split_list = line.split(" ")
            for i in range(0,len(split_list)):
                result = split_list[i].find("height=")
                if result != -1:
                    blocks_count = int(split_list[i].split("=")[1])
                    continue
                result = split_list[i].find("tx=")
                if result != -1:
                    txs_bitcoind = int(split_list[i].split("=")[1])
                    break
            if txs_bitcoind != 0:
                break    
    f.close()
    return txs_bitcoind

