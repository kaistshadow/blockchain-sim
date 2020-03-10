import os
from subprocess import check_output
import argparse

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)


def prepare_shadow():
    if os.path.exists(os.path.expanduser("~/.shadow/bin")):
        print "Shadow simulator is already installed"
        print "If you want to force re-installation, remove the installed shadow by erasing ~/.shadow directory"
        exit(0);
    else:
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

    # cloning shadow repository (submodule)
    exec_shell_cmd("git submodule init shadow")
    exec_shell_cmd("git submodule update shadow")

def prepare_nodejs():
    nodejs_serv_path = "./BLEEPeval/web-gui"    

    exec_shell_cmd("curl -sL https://deb.nodesource.com/setup_8.x | sudo -E bash -")
    exec_shell_cmd("sudo apt-get update")
    exec_shell_cmd("sudo apt-get install -y nodejs")
    exec_shell_cmd("cd %s; npm install websocket finalhandler serve-static jsonpath" % nodejs_serv_path)
    exec_shell_cmd("cd %s; npm install @maxmind/geoip2-node" % nodejs_serv_path)
    exec_shell_cmd("cd vis; npm install; npm run build; cd ..")


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

        ## install BLEEPlib
        exec_shell_cmd("mkdir build; cd build; cmake %s ../BLEEPlib/; make; make install; cd ..; rm -rf build" % cmake_debug_opt)

        ## install BLEEPapp
        exec_shell_cmd("mkdir build; cd build; cmake %s ../BLEEPapp/; make; make install; cd ..; rm -rf build" % cmake_debug_opt)

        ## install shadow
        exec_shell_cmd("mkdir build; cd build; cmake %s ../shadow; make; make install; cd ..; rm -rf build" % cmake_debug_opt)

        rcFile = os.path.expanduser("~/.bashrc")
        f = open(rcFile, 'r')
        shadowPath = "export PATH=$PATH:%s" % os.path.expanduser("~/.shadow/bin" )
        needWrite = True
        while True:
	    line = f.readline()
            if shadowPath in line:
                needWrite = False
                break
            if not line:
                break
        if needWrite:
            exec_shell_cmd("echo '%s' >> ~/.bashrc && . ~/.bashrc" % shadowPath)

        print "After installing the shadow, execute following commands on your bash. (type without dollor sign)"
        print "$ source ~/.bashrc"

    if OPT_TEST:
        exec_shell_cmd("mkdir build; cd build; cmake ../; make test")


