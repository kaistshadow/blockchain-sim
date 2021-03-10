import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)


def prepare_shadow():
    print ("Installing...")

    # install dependencies
    exec_shell_cmd("sudo apt-get install libc6-dbg")
    exec_shell_cmd("sudo apt-get install -y python python-pyelftools python-pip python-matplotlib python-numpy python-scipy python-networkx python-lxml")
    exec_shell_cmd("sudo apt-get install -y git dstat git screen htop libffi-dev libev-dev")
    exec_shell_cmd("sudo apt-get install -y gettext")
    # exec_shell_cmd("sudo pip install lxml") # User should install it manually when it is needed

    if "Ubuntu 14.04" in check_output(["bash", "-c", "cat /etc/lsb-release | grep DESCRIPTION"]):
        exec_shell_cmd("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0 libigraph0-dev cmake make xz-utils")
        print ("Installing glib manually...")
        exec_shell_cmd("wget http://ftp.gnome.org/pub/gnome/sources/glib/2.42/glib-2.42.1.tar.xz")
        exec_shell_cmd("tar xaf glib-2.42.1.tar.xz")
        exec_shell_cmd("cd glib-2.42.1; ./configure --prefix=%s; make; make install" % os.path.expanduser("~/.shadow"))
        exec_shell_cmd("rm -rf glib-*")
    elif "Ubuntu 16.04" in check_output(["bash", "-c", "cat /etc/lsb-release | grep DESCRIPTION"]):
        exec_shell_cmd("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils")
    else:
        exec_shell_cmd("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils")


def prepare_shadow_dependencies():
    exec_shell_cmd("sudo apt-get install -y libcurl4-openssl-dev")
    exec_shell_cmd("sudo apt-get update -y")
    exec_shell_cmd("sudo apt-get install -y libidn2-dev")
    # install dependencies for zeromq (zeromq is used for IPC implementation)
    exec_shell_cmd("sudo apt-get install -y libzmq3-dev")

def process_ENV():
    rcFile = os.path.expanduser("~/.bashrc")
    f = open(rcFile, 'r')
    shadowPath = "export PATH=$PATH:%s" % os.path.abspath("./Install/bin" )
    libPath = "export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:%s" % os.path.abspath("./Install")
    needWritePath = True
    needWriteLibPath = True
    exec_shell_cmd("sudo apt install python3-pip")
    exec_shell_cmd("pip3 install --upgrade lxml")
    for line in f:
        if shadowPath in line:
            needWritePath = False
        if libPath in line:
            needWriteLibPath = False
    if needWritePath:
        exec_shell_cmd("echo '%s' >> ~/.bashrc && . ~/.bashrc" % shadowPath)
    if needWriteLibPath:
        exec_shell_cmd("echo '%s' >> ~/.bashrc && . ~/.bashrc" % libPath)

    print ("After installing, execute following commands on your bash. (type without dollor sign)")
    print ("$ source ~/.bashrc")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--all", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--test", action="store_true", help="Run tests")
    parser.add_argument("--unittest", action="store_true", help="Run Unit tests")
    parser.add_argument("--debug", action="store_true", help="Include debug symbols for shadow")
    parser.add_argument("--bitcoin", action="store_true", help="only bitcoin build")
    parser.add_argument("--git", action="store_true", help="Run on Git action")
    

    args = parser.parse_args()

    OPT_BITCOIN = args.bitcoin

    OPT_GIT = args.git
    OPT_ALL = args.all
    OPT_DEBUG = args.debug
    OPT_TEST = args.test

    OPT_UNITTEST = args.unittest

    cmake_bleeplib_opt = "-DBLEEPLIB_OPT=ON"
    cmake_debug_opt = "-DSHADOW_DEBUG=ON -DBLEEP_DEBUG=ON"


    if len(sys.argv) == 1:
        exec_shell_cmd("git submodule update --init")
        #bitcoin dependency
        exec_shell_cmd("sudo apt-get install -y libboost-all-dev")
        exec_shell_cmd("sudo apt-get install -y autoconf libtool libevent-dev libdb++-dev")
        exec_shell_cmd("sudo apt-get install -y libssl-dev")
        prepare_shadow()
        prepare_shadow_dependencies()
        exec_shell_cmd("mkdir build; cd build; cmake %s %s ../; cmake --build . --target install -- -j 8; cd ..;" %(cmake_debug_opt, cmake_bleeplib_opt))
        process_ENV()   

    if OPT_DEBUG:
        cmake_debug_opt = "-DSHADOW_DEBUG=ON -DBLEEP_DEBUG=ON"

    if OPT_BITCOIN:
        exec_shell_cmd("git submodule update --init")
        #bitcoin dependency
        exec_shell_cmd("sudo apt-get install -y libboost-all-dev")
        exec_shell_cmd("sudo apt-get install -y autoconf libtool libevent-dev libdb++-dev")
        exec_shell_cmd("sudo apt-get install -y libssl-dev")
        #rpc client dependency
        exec_shell_cmd("sudo apt-get install -y libjsoncpp-dev")
        prepare_shadow()
        prepare_shadow_dependencies()
        exec_shell_cmd("mkdir build; cd build; cmake %s %s ../; cmake --build . --target install -- -j 8; cd ..;" %(cmake_debug_opt, cmake_bleeplib_opt))
        process_ENV()   

    if OPT_ALL:
        # cloning shadow repository (submodule)
        exec_shell_cmd("git submodule update --init")
        prepare_shadow()
        prepare_shadow_dependencies()

        # ## install boost-lib
        exec_shell_cmd("sudo apt-get install -y libboost-all-dev")
        exec_shell_cmd("sudo apt-get install -y libjsoncpp-dev")

        ## install bitcoin dependencies
        exec_shell_cmd("sudo apt-get install -y autoconf libtool libevent-dev libdb++-dev")
        ## bitcoin first run (without wallet enabled) dependencies
        exec_shell_cmd("sudo apt-get install -y libssl-dev")
        cmake_all_opt = "-DALL_OPT=ON"

        ## install
        exec_shell_cmd("mkdir build; cd build; cmake %s %s %s ../; cmake --build . --target install -- -j 8; cd ..;" % (cmake_debug_opt, cmake_all_opt, cmake_bleeplib_opt))
        process_ENV()
    
    if OPT_GIT:
         # cloning shadow repository (submodule)
        exec_shell_cmd("git submodule update --init")
        prepare_shadow()
        prepare_shadow_dependencies()
        ### Until the complete tests are done, let's exclude following external modules from git all build

        # ## install boost-lib
        exec_shell_cmd("sudo apt-get install -y libboost-all-dev")

        ## install bitcoin dependencies
        exec_shell_cmd("sudo apt-get install -y autoconf libtool libevent-dev libdb++-dev")
        ## bitcoin first run (without wallet enabled) dependencies
        exec_shell_cmd("sudo apt-get install -y libssl-dev")
        cmake_git_opt = "-DGIT_OPT=ON"

        ## install
        exec_shell_cmd("mkdir build; cd build; cmake %s %s %s ../; cmake --build . --target install -- -j 8; cd ..;" % (cmake_debug_opt, cmake_git_opt, cmake_bleeplib_opt))
        process_ENV()


    if OPT_TEST:
        cmake_test_opt = "-DTEST_OPT=ON"
        exec_shell_cmd("mkdir -p build; cd build; cmake %s ../; make -j8; make test" %(cmake_test_opt))

    if OPT_UNITTEST:
        exec_shell_cmd("git submodule update --init")
        cmake_unittest_opt = "-DUNITTEST_OPT=ON"
        exec_shell_cmd("mkdir -p build; cd build; cmake %s ../; make -j8" %(cmake_unittest_opt))


