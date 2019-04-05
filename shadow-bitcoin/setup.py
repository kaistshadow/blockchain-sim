import os
import lxml.etree as ET
from subprocess import check_output
import argparse

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)


def prepare_bitcoin_plugin():
    bitcoin_plugin_path = "./shadow-plugin-bitcoin"

    # cloning plugin repository (submodule)
    exec_shell_cmd("git submodule init shadow-plugin-bitcoin")
    exec_shell_cmd("git submodule update shadow-plugin-bitcoin")

    # cloning bitcoin repository (submodule)
    exec_shell_cmd("git -C %s submodule init" % bitcoin_plugin_path)
    exec_shell_cmd("git -C %s submodule update" % bitcoin_plugin_path)
    
    # install dependencies
    exec_shell_cmd("sudo apt-get install -y autoconf libtool libboost-all-dev libssl-dev libevent-dev")    


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--install", action="store_true", help="Install the bitcoin-plugin")
    parser.add_argument("--test", action="store_true", help="Run tests")

    args = parser.parse_args()
    OPT_INSTALL = args.install
    OPT_TEST = args.test

    if OPT_INSTALL:
        prepare_bitcoin_plugin()
        os.system("mkdir build; cd build; cmake ../; make; make install")

    if OPT_TEST:
        os.system("cd build; make test")
    
