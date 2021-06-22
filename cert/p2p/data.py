import os
import argparse
import sys
import time
import random

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)
#stored addr num:4
def get_stored_data(target_directory, i):
    # stdout-pownode0.NODE.1000.log
    target_directory = target_directory + "/" + "stdout-pownode" + str(i) + ".NODE.1000.log"
    f = open(target_directory, "r")
    for line in f.readlines()[::-1]:
        result = line.find("stored")
        if result != -1:
            f.close()
            return int(line.split(":")[1][:-1])
    f.close()

if __name__ == '__main__':
    
    stored_list = []
    path = os.path.abspath("./")
    target_path = path + "/shadow.data/hosts/pownode" 

    for i in range(0,1000):
        target_directory = target_path + str(i)
        print("pownode%d / %s" %(i, get_stored_data(target_directory,i)))
