import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)


def prepare_shadow():
    print "Installing..."

    # install dependencies
    exec_shell_cmd("sudo apt-get install libc6-dbg")
    exec_shell_cmd("sudo apt-get install -y python python-pyelftools python-pip python-matplotlib python-numpy python-scipy python-networkx python-lxml")
    exec_shell_cmd("sudo apt-get install -y git dstat git screen htop libffi-dev libev-dev")
    exec_shell_cmd("sudo apt-get install -y gettext")
    # exec_shell_cmd("sudo pip install lxml") # User should install it manually when it is needed

    if "Ubuntu 14.04" in check_output(["bash", "-c", "cat /etc/lsb-release | grep DESCRIPTION"]):
        exec_shell_cmd("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0 libigraph0-dev cmake make xz-utils")
        print "Installing glib manually..."
        exec_shell_cmd("wget http://ftp.gnome.org/pub/gnome/sources/glib/2.42/glib-2.42.1.tar.xz")
        exec_shell_cmd("tar xaf glib-2.42.1.tar.xz")
        exec_shell_cmd("cd glib-2.42.1; ./configure --prefix=%s; make; make install" % os.path.expanduser("~/.shadow"))
        exec_shell_cmd("rm -rf glib-*")
    elif "Ubuntu 16.04" in check_output(["bash", "-c", "cat /etc/lsb-release | grep DESCRIPTION"]):
        exec_shell_cmd("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils")
    else:
        exec_shell_cmd("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils")

def prepare_nodejs():
    nodejs_serv_path = "./BLEEPeval/web-gui"    

    exec_shell_cmd("curl -sL https://deb.nodesource.com/setup_10.x | sudo -E bash -")
    exec_shell_cmd("sudo apt-get update")
    exec_shell_cmd("sudo apt-get install -y nodejs")
    exec_shell_cmd("cd %s; npm install websocket finalhandler serve-static jsonpath" % nodejs_serv_path)
    exec_shell_cmd("cd %s; npm install @maxmind/geoip2-node" % nodejs_serv_path)
    exec_shell_cmd("cd vis; npm install; npm run build; cd ..")

def prepare_rust():
    exec_shell_cmd("sudo apt-get install -y rustc")

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--install", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--test", action="store_true", help="Run tests")
    parser.add_argument("--debug", action="store_true", help="Include debug symbols for shadow")

    args = parser.parse_args()
    OPT_INSTALL = args.install
    OPT_TEST = args.test
    OPT_DEBUG = args.debug

    if len(sys.argv) == 1:
        parser.print_help(sys.stderr)
        sys.exit(1)        

    cmake_debug_opt = "-DSHADOW_DEBUG=ON"
    if OPT_DEBUG:
        cmake_debug_opt = "-DSHADOW_DEBUG=ON"

    if OPT_INSTALL:
        # cloning shadow repository (submodule)
        exec_shell_cmd("git submodule update --init")

        prepare_shadow()
        prepare_nodejs()
        prepare_rust()

        ## install
        exec_shell_cmd("mkdir build; cd build; cmake %s ../; make; make install; cd ..; rm -rf build" % cmake_debug_opt)


        rcFile = os.path.expanduser("~/.bashrc")
        f = open(rcFile, 'r')
        shadowPath = "export PATH=$PATH:%s" % os.path.abspath("./Install/bin" )
        libPath = "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:%s" % os.path.abspath("./Install")
        needWritePath = True
        needWriteLibPath = True
        for line in f:
            if shadowPath in line:
                needWritePath = False
            if libPath in line:
                needWriteLibPath = False
        if needWritePath:
            exec_shell_cmd("echo '%s' >> ~/.bashrc && . ~/.bashrc" % shadowPath)
        if needWriteLibPath:
            exec_shell_cmd("echo '%s' >> ~/.bashrc && . ~/.bashrc" % libPath)

        print "After installing, execute following commands on your bash. (type without dollor sign)"
        print "$ source ~/.bashrc"

    if OPT_TEST:
        exec_shell_cmd("mkdir build; cd build; cmake ../; make test")


