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
        hours = target_time/3600
        mins = (target_time%3600)/60
        sec = (target_time%3600)%60
    elif target_time < 3600:
        mins = (target_time%3600)/60
        sec = (target_time%3600)%60

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
                sys.exit(0)
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

# Test process
# 1. Test - xml file existence 
# 2. Run shadow and get shadow.output
# 3. Get infos such as simulation runtime, plugin id from xml file
# 4. Test - shadow output file existence
# 5. Test - shadow
def main():
    # add xml generator
    target_folder_xml = test_modules.test_xml_existence("output.xml")
    subprocess_open('shadow output.xml > output.txt')
    runtime, node_id_list, plugin_list = test_modules.get_xml_info(target_folder_xml)
    target_folder_file = test_shadow_output_file_existence()
    test_shadow(target_folder_file, runtime, node_id_list)

if __name__ == '__main__':
    main()