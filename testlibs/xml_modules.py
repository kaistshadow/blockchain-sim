#
# 2021-03-15
# created by hong joon
#
import os
from subprocess import check_output
import argparse
import sys
import os
import lxml.etree as ET
import subprocess
import math
sys.path.append("")
from testlibs import utils

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

# Description : xml 파일을 생성해주는 함수.
# input param : 함수가 실행되는 위치의 경로
# output : tx_mode (enable/disable) 
def get_xmlfile(path):
    tx_mode = ""
    emulation_start, path_abs = utils.path_filter(path)
    node_count = 0
    the_command = path_abs + "; python xmlGenerator.py"
    tx_condition_count = 0
    condition_count = -1
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
        
        if condition_count == -1:
            try:
                node_count = int(input("Input node count (only integer more than 1) : "))
                if (node_count < 0) | (node_count == 0):
                    print("Only input integer ... (more than 1)")
                else:
                    node_count = str(node_count)
                    condition_count = 0
            except:
                print("Only input integer ... (more than 1) ")
                continue
            
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
                    xml_command = the_command + " " + node_count + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty + " " + path
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
                    print("Enter only one of them (enable/disable) ")
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
                        xml_command = the_command + " " + node_count + " " + sim_time + " " + algo + " " + tx_mode + " " + difficulty + " " + str(tx_cnt) +" " + str(tx_sec) + " " + str(number_bitcoins_transferred) + " " + path
                        break
                    else:
                        print("Must input only number ! ")
                        continue
                except ValueError as e:
                    print("Must input only number ! ")
                    continue

    exec_shell_cmd(xml_command)
    print("-----------------------------------------------------------------------------------------------------------------------------")
    return tx_mode

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