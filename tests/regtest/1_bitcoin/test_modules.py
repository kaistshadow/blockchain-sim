import os
from subprocess import check_output
import argparse
import sys

def test_xml_existence(output):
    path = os.path.abspath(".")
    target_folder = path + "/" + output
    if os.path.isfile(target_folder):
        print("Success xml existence test ...")
        return target_folder
    else:
        print("Fail xml existence test ...")
        sys.exit(1)

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
            print("Fail not existence file - %s", %(target_folder_list[i]))
            sys.exit(1)
    return target_folder_list