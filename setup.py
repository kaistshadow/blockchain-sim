import os
import lxml.etree as ET
from subprocess import check_output
import argparse

def prepare_shadow():
    if os.path.exists(os.path.expanduser("~/.shadow/bin")):
        print "Shadow simulator is already installed"
        print "If you want to force re-installation, remove the installed shadow by erasing ~/.shadow directory"
        exit(0);
    else:
        print "Installing..."
        
        # install dependencies
        os.system("sudo apt-get install libc-dbg")
        os.system("sudo apt-get install -y python python-pip python-matplotlib python-numpy python-scipy python-networkx python-lxml")
        os.system("sudo apt-get install -y git dstat screen htop libffi-dev libev-dev")
        os.system("sudo pip install lxml")

        if "Ubuntu 14.04" in check_output(["bash", "-c", "cat /etc/lsb-release | grep DESCRIPTION"]):
            os.system("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0 libigraph0-dev cmake make xz-utils")
            print "Installing glib manually..."
            os.system("wget http://ftp.gnome.org/pub/gnome/sources/glib/2.42/glib-2.42.1.tar.xz")
            os.system("tar xaf glib-2.42.1.tar.xz")
            os.system("cd glib-2.42.1; ./configure --prefix=%s; make; make install" % os.path.expanduser("~/.shadow"))
        elif "Ubuntu 16.04" in check_output(["bash", "-c", "cat /etc/lsb-release | grep DESCRIPTION"]): 
            # currently, 16.04 also needs glib installation
            os.system("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils")
            print "Installing glib manually..."
            os.system("wget http://ftp.gnome.org/pub/gnome/sources/glib/2.42/glib-2.42.1.tar.xz")
            os.system("tar xaf glib-2.42.1.tar.xz")
            os.system("cd glib-2.42.1; ./configure --prefix=%s; make; make install" % os.path.expanduser("~/.shadow"))
        else:
            os.system("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils")

        # cloning shadow repository (submodule)
        os.system("git submodule init shadow")
        os.system("git submodule update shadow")

def prepare_nodejs():
    nodejs_serv_path = "./test/BLEEP/web-gui"    

    os.system("curl -sL https://deb.nodesource.com/setup_8.x | sudo -E bash -")
    os.system("sudo apt-get update")
    os.system("sudo apt-get install -y nodejs")
    os.system("cd %s; npm install websocket finalhandler serve-static vis jsonpath" % nodejs_serv_path)
    os.system("cd %s; npm install @maxmind/geoip2-node" % nodejs_serv_path)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--install", action="store_true", help="Install the shadow simulator and BLEEP")
    parser.add_argument("--test", action="store_true", help="Run tests")
    parser.add_argument("--debug", action="store_true", help="Include debug symbols for shadow")

    args = parser.parse_args()
    OPT_INSTALL = args.install
    OPT_TEST = args.test
    OPT_DEBUG = args.debug
    
    cmake_debug_opt = "-DSHADOW_DEBUG=ON"
    if OPT_DEBUG:
        cmake_debug_opt = "-DSHADOW_DEBUG=ON"
    
    if OPT_INSTALL:
        prepare_shadow()
        prepare_nodejs()
        os.system("mkdir build; cd build; cmake %s ../; make; make install" % cmake_debug_opt)
        os.system("echo 'export PATH=$PATH:%s' >> ~/.bashrc && . ~/.bashrc" % os.path.expanduser("~/.shadow/bin"))
        print "After installing the shadow, execute following commands on your bash. (type without dollor sign)"
        print "$ source ~/.bashrc"

    if OPT_TEST:
        os.system("cd build; make test")


    
