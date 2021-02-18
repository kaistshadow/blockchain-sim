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
    target_folder_bitcoin = path + "/shadow.data/hosts/bcdnode0/stdout-bcdnode0.bitcoind.1000.log"
    target_folder_rpc = path + "/shadow.data/hosts/client0/stdout-client0.rpc.1000.log"
    return_count = 0
    if os.path.isfile(target_folder_bitcoin):
        f = open(target_folder_bitcoin, "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find("getnewaddress")
            if result != -1:
                return_count += 1
        f.close()
        if return_count == 0:
            sys.exit(1)
        if os.path.isfile(target_folder_rpc):
            f = open(target_folder_rpc, "r")
            line = f.readline().strip()
            f.close()
            if len(line) == 35:
                sys.exit(0)
            else:
                sys.exit(1)
    else:
        sys.exit(1)
if __name__ == '__main__':
    main()