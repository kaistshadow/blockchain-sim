import os
import subprocess
import argparse
import sys
import shlex
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
import test_modules

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

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
                    difficulty = str(split_list[i].split("=")[2])
                    break
        if count != 0:
            break
    f.close()
    return difficulty

def test_difficulty_compare(bitcoin_log, xml_difficulty):
    difficulty = ""
    f = open(bitcoin_log, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("difficulty")
        if result != -1:
            split_list = line.split(",")[4]
            difficulty = split_list.split(":")[1]
            break
    f.close()

    if difficulty == xml_difficulty:
        print("Success difficulty test ...")
        sys.exit(0)
    else:
        print("Fail difficulty test ...")
        sys.exit(1)
            
def main():
    # add xml generator
    target_folder_xml = test_modules.test_xml_existence("output.xml")
    # exec_shell_cmd(target_folder_xml)
    xml_difficulty = get_difficulty_info(target_folder_xml)
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    test_difficulty_compare(simulation_output_file[1], xml_difficulty)

if __name__ == '__main__':
    main()