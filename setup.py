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
    exec_shell_cmd("sudo apt-get install -y curl")
    exec_shell_cmd("curl -sL https://deb.nodesource.com/setup_10.x | sudo -E bash -")
    exec_shell_cmd("sudo apt-get update")
    exec_shell_cmd("sudo apt-get install -y nodejs")
    exec_shell_cmd("cd %s; npm install websocket finalhandler serve-static jsonpath" % nodejs_serv_path)
    exec_shell_cmd("cd %s; npm install @maxmind/geoip2-node" % nodejs_serv_path)
    exec_shell_cmd("cd vis; npm install; npm run build; cd ..")
    
def prepare_monero_dependencies():
    exec_shell_cmd("sudo apt-get install build-essential cmake pkg-config libboost-all-dev libssl-dev libzmq3-dev libunbound-dev libsodium-dev libpgm-dev ")

def prepare_eos_dependencies():
    EOSIO_LOCATION = os.getcwd()+"/external/eosio/eos"
    EOSIO_INSTALL_LOCATION = EOSIO_LOCATION+"/../install"
    if not os.path.isdir(EOSIO_LOCATION):
        os.mkdir(EOSIO_LOCATION)
    if not os.path.isdir(EOSIO_INSTALL_LOCATION):
        os.mkdir(EOSIO_INSTALL_LOCATION)
    exec_shell_cmd("sudo apt-get install libstdc++-8-dev g++-7-multilib")
    exec_shell_cmd("sudo apt-get install -y make bzip2 automake libbz2-dev libssl-dev doxygen graphviz libgmp3-dev autotools-dev libicu-dev python2.7 python2.7-dev python3 python3-dev autoconf libtool curl zlib1g-dev sudo ruby libusb-1.0-0-dev libcurl4-gnutls-dev pkg-config patch llvm-7-dev clang-7 vim-common jq")
    #build cmake
    if not os.path.isdir(EOSIO_INSTALL_LOCATION+"/doc/cmake-3.13"):
        exec_shell_cmd("cd %s; curl -LO https://cmake.org/files/v3.13/cmake-3.13.2.tar.gz" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s; tar -xzf cmake-3.13.2.tar.gz" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/cmake-3.13.2; ./bootstrap --prefix=%s" % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
        exec_shell_cmd("cd %s/cmake-3.13.2; make -j$(nproc)" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/cmake-3.13.2; make install" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("rm -rf %s/cmake-3.13.2.tar.gz %s/cmake-3.13.2" % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
    else:
        print("== Skip build cmake")
    #build boost
    if not os.path.isdir(EOSIO_INSTALL_LOCATION+"/include/boost"):
        exec_shell_cmd("cd %s; curl -LO https://dl.bintray.com/boostorg/release/1.71.0/source/boost_1_71_0.tar.bz2" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s; tar -xjf boost_1_71_0.tar.bz2" % EOSIO_INSTALL_LOCATION)
        # add -fPIC
        exec_shell_cmd("cp -f %s/../gcc.jam %s/boost_1_71_0/tools/build/src/tools/" % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
        exec_shell_cmd("cd %s/boost_1_71_0; ./bootstrap.sh --prefix=%s" % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
        exec_shell_cmd("cd %s/boost_1_71_0; ./b2 --with-iostreams --with-date_time --with-filesystem --with-system --with-program_options --with-chrono --with-test -q -j$(nproc) install" % (EOSIO_INSTALL_LOCATION))
        exec_shell_cmd("rm -rf %s/boost_1_71_0.tar.bz2 %s/boost_1_71_0" % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
    else:
        print("== Skip build boost")
    #build mongodb
    if not os.path.isfile(EOSIO_INSTALL_LOCATION+"/bin/mongo"):
        exec_shell_cmd("cd %s; curl -LO https://fastdl.mongodb.org/linux/mongodb-linux-x86_64-ubuntu1804-4.1.1.tgz" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s; tar -xzf mongodb-linux-x86_64-ubuntu1804-4.1.1.tgz " % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s; rm -f mongodb-linux-x86_64-ubuntu1804-4.1.1.tgz" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("mv %s/mongodb-linux-x86_64-ubuntu1804-4.1.1/bin/* %s/bin" % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
        exec_shell_cmd("rm -rf %s/mongodb-linux-x86_64-ubuntu1804-4.1.1" % EOSIO_INSTALL_LOCATION)
    else:
        print ("== Skip build mongodb")
    # build mongodb c driver
    if not os.path.isdir(EOSIO_INSTALL_LOCATION+"/share/mongo-c-driver"):
        exec_shell_cmd("cd %s; curl -LO https://github.com/mongodb/mongo-c-driver/releases/download/1.13.0/mongo-c-driver-1.13.0.tar.gz" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s; tar -xzf mongo-c-driver-1.13.0.tar.gz" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/mongo-c-driver-1.13.0; mkdir -p build" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/mongo-c-driver-1.13.0/build; cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_INSTALL_PREFIX=%s -DENABLE_BSON=ON -DENABLE_SSL=OPENSSL -DENABLE_AUTOMATIC_INIT_AND_CLEANUP=OFF -DENABLE_STATIC=ON -DENABLE_ICU=OFF -DENABLE_SNAPPY=OFF .. " % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
        exec_shell_cmd("cd %s/mongo-c-driver-1.13.0/build; make -j$(nproc)" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/mongo-c-driver-1.13.0/build; make install" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("rm -rf %s/mongo-c-driver-1.13.0.tar.gz %s/mongo-c-driver-1.13.0" % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
    else:
        print("== Skip build mongodb_c_driver")
    # build static mongodb cxx driver
    if not os.path.isfile(EOSIO_INSTALL_LOCATION+"/lib/libmongocxx-static.a"):
        exec_shell_cmd("cd %s; curl -L https://github.com/mongodb/mongo-cxx-driver/archive/r3.4.0.tar.gz -o mongo-cxx-driver-r3.4.0.tar.gz" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s; tar -xzf mongo-cxx-driver-r3.4.0.tar.gz" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/mongo-cxx-driver-r3.4.0; sed -i 's/\"maxAwaitTimeMS\", count/\"maxAwaitTimeMS\", static_cast<int64_t>(count)/' src/mongocxx/options/change_stream.cpp" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/mongo-cxx-driver-r3.4.0; sed -i 's/add_subdirectory(test)//' src/mongocxx/CMakeLists.txt src/bsoncxx/CMakeLists.txt " % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/mongo-cxx-driver-r3.4.0; mkdir -p build" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/mongo-cxx-driver-r3.4.0/build; cmake -DBUILD_SHARED_LIBS=OFF -DCMAKE_POSITION_INDEPENDENT_CODE=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_INSTALL_PREFIX=%s .." % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
        exec_shell_cmd("cd %s/mongo-cxx-driver-r3.4.0/build; make -j$(nproc)" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("cd %s/mongo-cxx-driver-r3.4.0/build; make install" % EOSIO_INSTALL_LOCATION)
        exec_shell_cmd("rm -rf %s/mongo-cxx-driver-r3.4.0.tar.gz %s/mongo-cxx-driver-r3.4.0" % (EOSIO_INSTALL_LOCATION,EOSIO_INSTALL_LOCATION))
    else:
        print("== Skip build static mongodb_cxx_driver")

def prepare_rust():
    exec_shell_cmd("sudo apt-get install -y rustc")

    # Following script is available for rustup installation.
    # However, shadow plugin is not compatible for rust library compiled by rustup-installed rustc
    # So, we commentify following sciprt

    # exec_shell_cmd("curl https://sh.rustup.rs -sSf | sh -s -- -y")
    # exec_shell_cmd("rustup toolchain install 1.39.0")
    # exec_shell_cmd("rustup default 1.39.0")
    #
    # rcFile = os.path.expanduser("~/.bashrc")
    # f = open(rcFile, 'r')
    # rustPath = "export PATH=$PATH:%s" % os.path.expanduser("~/.cargo/bin" )
    # needWriteRustPath = True
    # for line in f:
    #     if rustPath in line:
    #         needWriteRustPath = False
    # if needWriteRustPath:
    #     exec_shell_cmd("echo '%s' >> ~/.bashrc" % rustPath)

def prepare_golang():
    if not os.path.exists("/usr/bin/go"):
        exec_shell_cmd("sudo apt-get install -y golang")
    exec_shell_cmd("sudo apt-get install -y gccgo")

def prepare_shadow_dependencies():
    exec_shell_cmd("sudo apt-get install libcurl4-openssl-dev")
    exec_shell_cmd("sudo apt-get update -y")
    exec_shell_cmd("sudo apt-get install -y libidn2-dev")
    # install dependencies for zeromq (zeromq is used for IPC implementation)
    exec_shell_cmd("sudo apt-get install -y libzmq3-dev")

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

    cmake_debug_opt = "-DSHADOW_DEBUG=ON -DBLEEP_DEBUG=ON"
    if OPT_DEBUG:
        cmake_debug_opt = "-DSHADOW_DEBUG=ON -DBLEEP_DEBUG=ON"

    if OPT_INSTALL:
        # cloning shadow repository (submodule)
        exec_shell_cmd("git submodule update --init")

        prepare_shadow()
#         prepare_nodejs()
        prepare_rust()
        prepare_golang()
        prepare_shadow_dependencies()
        prepare_monero_dependencies()
        prepare_eos_dependencies()

        ## install boost-lib
        exec_shell_cmd("sudo apt-get install -y libboost-all-dev")

        ## install bitcoin dependencies
        exec_shell_cmd("sudo apt-get install -y autoconf libtool libevent-dev libdb++-dev")
        ## bitcoin first run (without wallet enabled) dependencies
        exec_shell_cmd("sudo apt-get install -y libssl-dev")

        ## install
        exec_shell_cmd("mkdir build; cd build; cmake %s ../; cmake --build . --target install -- -j 8; cd ..;" % cmake_debug_opt)


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
        exec_shell_cmd("mkdir -p build; cd build; cmake ../; make -j8; make test")


