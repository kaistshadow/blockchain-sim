import os
import subprocess
import argparse
import sys
import math

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
def get_xml_info(xml_file):
    split_result = []
    split_result2 = []
    node_id_list = []
    plugin_list = []

    f = open(xml_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("kill time")
        if result != -1:
            split_result = line.split('"')
        result = line.find("node id")
        if result != -1:
            result = line.find("poi")
            if result == -1:
                split_result2 = line.split('"')
                node_id_list.append(split_result2[1])
        result = line.find("application plugin")
        if result != -1:
            plugin_list_target = line.split('"')
            plugin_list.append(plugin_list_target[1])
    f.close()
    return split_result[1], node_id_list, plugin_list

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

# get xml file 
def get_xmlfile():
    sim_time = input("Input simulation time : ")
    algo = input("Input mining algorithm(pow/coinflip) : ")
    tx_mode = str(input("Input transaction injector (transaction/normal) : "))
    difficulty = input("Input difficulty(1/2/3) : ")
    xml_command = "./addexam.sh" + " " + "1" + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty
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
    txs_bitcoind = txs_bitcoind - blocks_count - 1

    txs = 0
    f = open(simulation_output_file[0], "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("getmempoolinfo")
        if result != -1:
            break
        result = line.find("sendtoaddress")
        if result != -1:
            txs += 1
    f.close()

    f = open(simulation_output_file[1], "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("mempool")
        if result != -1:
            mempool_size = line.split(",")[1].split(":")[1]
            break
    f.close()

    if txs_bitcoind + int(mempool_size) == txs:
        return txs
    else:
        sys.exit(1)