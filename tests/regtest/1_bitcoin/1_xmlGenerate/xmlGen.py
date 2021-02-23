import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def main():

    print("Input node id flag")
    print("Input network flags")
    print("Input application flags")

if __name__ == '__main__':
    main()