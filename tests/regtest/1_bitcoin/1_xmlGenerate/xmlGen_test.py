import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def main():

    print("----------------TODO-------------------")
    exec_shell_cmd("shadow output.xml")
    print("---------------------------------------")
    path = os.path.abspath(".")
    target_folder = path + "/shadow.data/hosts/bcdnode0/stdout-bcdnode0.bitcoind.1000.log"
    if os.path.isfile(target_folder):
        sys.exit(0)
    else:
        sys.exit(1)
if __name__ == '__main__':
    main()