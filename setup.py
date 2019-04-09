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


def setup_multiple_node_xml(node_num):
    baseline_xml = "plugins/shadow-plugin-bitcoin/resource/example.xml"
    new_xml = "plugins/shadow-plugin-bitcoin/resource/example_multiple_generated.xml"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)

    tree = ET.parse(baseline_xml, parser)

    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)

    # node = ET.SubElement(shadow, "node", id="bcdnode0")
    # ET.SubElement(node, "application", plugin="bitcoind", time="5", arguments="-datadir=./data/bcdnode0 -debug -printtoconsole -disablewallet=1 -server=0")

    for i in range(node_num):
        node_id = "bcdnode%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)            
        time = str(5 + i/100)
        if i % 2 == 0:
            ET.SubElement(node, "application", plugin="bitcoind", time=time, arguments="-datadir=./data/bcdnode%d -debug -printtoconsole -listen -connect=bcdnode%d -disablewallet=1 -server=0"%(i, i+1))
        else:
            ET.SubElement(node, "application", plugin="bitcoind", time=time, arguments="-datadir=./data/bcdnode%d -debug -printtoconsole -listen -connect=bcdnode%d -disablewallet=1 -server=0"%(i, i-1))

    tree.write(new_xml, pretty_print=True)
    
def setup_injector_plugin():
    injector_plugin_path = "./injectors/shadow-injector-bitcoin"
    os.system("cd %s; mkdir build; cd build; cmake ../ -DSHADOW=ON; make; make install" % injector_plugin_path)
    
def run_shadow_bitcoin_injector_example():
    run_path = "plugins/shadow-plugin-bitcoin/run"
    os.system("mkdir -p %s" % run_path)
    os.system("mkdir -p %s/data/bcdnode1; mkdir -p %s/data/bcdnode2;" % (run_path, run_path))
    os.system("cd %s; shadow ../resource/example_injector.xml" % run_path)
    
def run_shadow_bitcoin_multiple_node(node_num, worker_num):
    run_path = "plugins/shadow-plugin-bitcoin/run"
    os.system("rm -rf %s/data/bcdnode*" % run_path)
    for i in range(node_num):
        os.system("mkdir -p %s/data/bcdnode%d" % (run_path, i))
    os.system("mkdir %s/shadow.result" % run_path)
    os.system("cd %s; shadow -w %d ../resource/%s | tee shadow.result/shadow_%d_%d.result" % (run_path, worker_num, "example_multiple_generated.xml", node_num, worker_num))

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--install", action="store_true", help="Install the shadow simulator and bitcoin-plugin")
    parser.add_argument("--injector", action="store_true", help="Run bitcoin-plugin and injector-plugin")
    parser.add_argument("--test", action="store_true", help="Run tests")
    parser.add_argument("--debug", action="store_true", help="Include debug symbols for shadow")

    args = parser.parse_args()
    OPT_INSTALL = args.install
    OPT_INJECTOR = args.injector
    OPT_TEST = args.test
    OPT_DEBUG = args.debug
    
    cmake_debug_opt = "-DSHADOW_DEBUG=ON"
    if OPT_DEBUG:
        cmake_debug_opt = "-DSHADOW_DEBUG=ON"
    
    if OPT_INSTALL:
        prepare_shadow()
        # prepare_nodejs()
        os.system("mkdir build; cd build; cmake %s ../; make; make install" % cmake_debug_opt)
        os.system("echo 'export PATH=$PATH:%s' >> ~/.bashrc && . ~/.bashrc" % os.path.expanduser("~/.shadow/bin"))
        print "After installing the shadow, execute following commands on your bash. (type without dollor sign)"
        print "$ source ~/.bashrc"

    if OPT_INJECTOR:
        setup_injector_plugin()
        run_shadow_bitcoin_injector_example()

    if OPT_TEST:
        os.system("cd build; make test")


    ### Following commands are example for the experiment of multiple bitcoin nodes.
    # setup_multiple_node_xml(100)
    # run_shadow_bitcoin_multiple_node(100, 1)

    
