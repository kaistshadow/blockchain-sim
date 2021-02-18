import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def main():

    exec_shell_cmd("shadow output.xml")
    path = os.path.abspath(".")
    target_folder_bitcoin = path + "/shadow.data/hosts/bcdnode0/stdout-bcdnode0.bitcoind.1000.log"
    return_count = 0
    if os.path.isfile(target_folder_bitcoin):
        f = open(target_folder_bitcoin, "r")
        while True:
            line = f.readline()
            if not line: break
            # "height=1" means mine is activated
            result = line.find("height=0")
            if result != -1:
                sys.exit(0)
    else:
        sys.exit(1)
    sys.exit(1)
if __name__ == '__main__':
    main()