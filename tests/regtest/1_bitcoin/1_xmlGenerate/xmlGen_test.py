import os
from subprocess import check_output
import argparse
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
import test_modules

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def main():
    exec_shell_cmd("python xmlGen.py")
    test_modules.test_xml_existence("output.xml")

if __name__ == '__main__':
    main()