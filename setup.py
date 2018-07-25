import os
import lxml.etree as ET
from subprocess import check_output
import argparse

def setup_shadow():
    if os.path.exists(os.path.expanduser("~/.shadow/bin")):
        print "Shadow simulator is already installed"
        print "If you want to force re-installation, remove the installed shadow by erasing ~/.shadow directory"
    else:
        print "Installing..."
        os.system("sudo apt-get install libc-dbg")
        os.system("sudo apt-get install -y python python-matplotlib python-numpy python-scipy python-networkx python-lxml")
        os.system("sudo apt-get install -y git dstat screen htop libffi-dev")

        if "Ubuntu 14.04" in check_output(["bash", "-c", "cat /etc/lsb-release | grep DESCRIPTION"]):
            os.system("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0 libigraph0-dev cmake make xz-utils")
            print "Installing glib manually..."
            os.system("wget http://ftp.gnome.org/pub/gnome/sources/glib/2.42/glib-2.42.1.tar.xz")
            os.system("tar xaf glib-2.42.1.tar.xz")
            os.system("cd glib-2.42.1; ./configure --prefix=%s; make; make install" % os.path.expanduser("~/.shadow"))
            os.system("rm -rf glib-2.42.1 glib-2.42.1.tar.xz")
        else:
            os.system("sudo apt-get install -y gcc g++ libglib2.0-0 libglib2.0-dev libigraph0v5 libigraph0-dev cmake make xz-utils")

        
        if not os.path.exists("./shadow"):
            os.system("git clone https://github.com/kaistshadow/shadow.git")
        os.system("cd shadow; ./setup build --clean --debug --test")
        os.system("cd shadow; ./setup install")
        os.system("echo 'export PATH=$PATH:%s' >> ~/.bashrc && . ~/.bashrc" % os.path.expanduser("~/.shadow/bin"))
    

def setup_bitcoin_plugin():
    bitcoin_plugin_path = "./plugins/shadow-plugin-bitcoin"

    if not os.path.exists(bitcoin_plugin_path):
        os.system("mkdir -p %s" % bitcoin_plugin_path)
        os.system("git clone https://github.com/kaistshadow/shadow-plugin-bitcoin %s" % bitcoin_plugin_path)
        os.system("mkdir -p %s/build" % bitcoin_plugin_path)
    
    os.system("sudo apt-get install -y autoconf libtool libboost-all-dev libssl-dev libevent-dev")    

    bitcoin_path = "./plugins/shadow-plugin-bitcoin/build/bitcoin"
    if not os.path.exists(bitcoin_path):
        os.system("mkdir -p %s" % bitcoin_path)
        os.system("git clone https://github.com/bitcoin/bitcoin.git %s" % bitcoin_path)
    
    os.system("git -C %s checkout ." % bitcoin_path)
    os.system("git -C %s checkout v0.16.0" % bitcoin_path)
    os.system("git -C %s clean -d -f -x" % bitcoin_path)
    os.system("cd %s; ./autogen.sh; ./configure --disable-wallet --enable-debug; make -C src obj/build.h; make -C src/secp256k1 src/ecmult_static_context.h"% bitcoin_path)
    
    # compile and install bitcoin plugin
    if "v0.15.0" in check_output(["git", "-C", bitcoin_path, "status"]):
        if not os.path.exists("plugins/shadow-plugin-bitcoin/DisableSanityCheck_v0.15.0.patch") or not os.path.exists("plugins/shadow-plugin-bitcoin/HandleGeneratePacket_v0.15.0.patch"):
            print "No appropriate patch exists"
            exit(1)
        else:
            os.system("cd %s; git apply ../../DisableSanityCheck_v0.15.0.patch" % bitcoin_path)
            os.system("cd %s; git apply ../../HandleGeneratePacket_v0.15.0.patch" % bitcoin_path)
    elif "v0.16.0" in check_output(["git", "-C", bitcoin_path, "status"]):
        if not os.path.exists("plugins/shadow-plugin-bitcoin/DisableSanityCheck_v0.16.0.patch") or not os.path.exists("plugins/shadow-plugin-bitcoin/HandleGeneratePacket_v0.16.0.patch"):
            print "No appropriate patch exists"
            exit(1)
        else:
            os.system("cd %s; git apply ../../DisableSanityCheck_v0.16.0.patch" % bitcoin_path)
            os.system("cd %s; git apply ../../HandleGeneratePacket_v0.16.0.patch" % bitcoin_path)
        

    build_path = "plugins/shadow-plugin-bitcoin/build"
    os.system("cd %s; cmake ../; make; make install" % build_path)
    
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
    
def run_shadow_bitcoin_example():
    run_path = "plugins/shadow-plugin-bitcoin/run"
    os.system("mkdir -p %s" % run_path)
    os.system("mkdir -p %s/data/bcdnode1; mkdir -p %s/data/bcdnode2;" % (run_path, run_path))
    os.system("cd %s; shadow ../resource/example.xml" % run_path)

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

    args = parser.parse_args()
    OPT_INSTALL = args.install
    OPT_INJECTOR = args.injector
    
    if OPT_INSTALL:
        setup_shadow()
        setup_bitcoin_plugin()

    if OPT_INJECTOR:
        setup_injector_plugin()
        run_shadow_bitcoin_injector_example()
    else:
        run_shadow_bitcoin_example()

    # temporary code : need to be improved later
    print "If shadow is not found, execute following commands on your bash. (type without dollor sign)"
    print "$ source ~/.bashrc"


    ### Following commands are example for the experiment of multiple bitcoin nodes.
    # setup_multiple_node_xml(100)
    # run_shadow_bitcoin_multiple_node(100, 1)

    
