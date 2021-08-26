import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)
    
def run_cpplint(target_list):
    for i in range(0,len(target_list)):
        result = target_list[i].find("./build")
        if result != -1:
            continue
        result = target_list[i].find("./external")
        if result != -1:
            continue
        result = target_list[i].find("./shadow")
        if result != -1:
            continue
        result = target_list[i].find("./testlibs")
        if result != -1:
            continue
        result = target_list[i].find("./tests")
        if result != -1:
            continue
        result = target_list[i].find("./interfaces")
        if result != -1:
            continue

        command_target = "cpplint --quiet --linelength=500 --filter=-build/c++11,-readability/inheritance,-runtime/explicit,-build/include_order,-build/include_subdir,-legal/copyright,-build/namespaces,-runtime/int,-readability/todo,-runtime/printf,-readability/casting,-runtime/references,-runtime/threadsafe_fn,-build/include " + target_list[i]
        exec_shell_cmd(command_target)
        

if __name__ == '__main__':
    cpp_file_list  = os.popen('find . -name "*.cpp"').readlines()
    header_file_list = os.popen('find . -name "*.h"').readlines()
    hpp_file_list = os.popen('find . -name "*.hpp"').readlines()

    print("Run cpplint ...")
    run_cpplint(cpp_file_list)
    run_cpplint(header_file_list)
    run_cpplint(hpp_file_list)
    print("Success cpplint ...")
