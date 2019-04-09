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

def setup_injector_plugin():
    injector_plugin_path = "./injectors/shadow-injector-bitcoin"
    exec_shell_cmd("cd %s; mkdir build; cd build; cmake ../ -DSHADOW=ON; make; make install" % injector_plugin_path)

def run_shadow_bitcoin_injector_example():
    run_path = "./shadow-plugin-bitcoin/run"
    exec_shell_cmd("mkdir -p %s" % run_path)
    exec_shell_cmd("mkdir -p %s/data/bcdnode1; mkdir -p %s/data/bcdnode2;" % (run_path, run_path))
    exec_shell_cmd("cd %s; shadow ../resource/example_injector.xml" % run_path)
    

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
    
    
def run_shadow_bitcoin_multiple_node(node_num, worker_num):
    run_path = "plugins/shadow-plugin-bitcoin/run"
    exec_shell_cmd("rm -rf %s/data/bcdnode*" % run_path)
    for i in range(node_num):
        exec_shell_cmd("mkdir -p %s/data/bcdnode%d" % (run_path, i))
    exec_shell_cmd("mkdir %s/shadow.result" % run_path)
    exec_shell_cmd("cd %s; shadow -w %d ../resource/%s | tee shadow.result/shadow_%d_%d.result" % (run_path, worker_num, "example_multiple_generated.xml", node_num, worker_num))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--install", action="store_true", help="Install the bitcoin-plugin")
    parser.add_argument("--test", action="store_true", help="Run tests")
    parser.add_argument("--injector", action="store_true", help="Run bitcoin-plugin and injector-plugin")


    args = parser.parse_args()
    OPT_INSTALL = args.install
    OPT_TEST = args.test
    OPT_INJECTOR = args.injector


    if OPT_INSTALL:
        prepare_bitcoin_plugin()
        exec_shell_cmd("mkdir build; cd build; cmake ../; make; make install")

    if OPT_TEST:
        exec_shell_cmd("cd build; make test")
    
    if OPT_INJECTOR:
        setup_injector_plugin()
        run_shadow_bitcoin_injector_example()

    ### Following commands are example for the experiment of multiple bitcoin nodes.
    # setup_multiple_node_xml(100)
    # run_shadow_bitcoin_multiple_node(100, 1)

