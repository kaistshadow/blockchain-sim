import os
from subprocess import check_output, Popen, PIPE
import argparse
import sys
import lxml.etree as ET

def setup_multiple_node_xml(node_num, simultime, bool_, storage_usuage, algorithm):
    base_xml = "example.xml"
    new_xml  = "base"+str(node_num)+"N"+str(simultime)+"T.xml"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)
    tree   = ET.parse(base_xml, parser)
    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)
    for node in shadow.findall('kill'):
        shadow.remove(node)
    ET.SubElement(shadow, "kill", time=str(simultime))

    for i in range(0, node_num):
        node_id = "bcdnode%d" % (i)
        node_iphint = "%d.%d.0.1" % (i/256 + 1, i%256)
        node = ET.SubElement(shadow, "node", id=node_id, iphint=node_iphint)
        time = str(0)
        if i==0:
            argument = "-debug -testnet -datadir=data/bcdnode%d -port=18333 -txindex=1 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=%s/0 -rpcbind=%s -addnode=%d.%d.0.1:18333 -addnode=%d.%d.0.1:18333 -storageShare=%s -algorithm=%s" % (i, (node_iphint), (node_iphint), (node_num-1)/256 + 1, (node_num-1)%256, (i+1)/256 + 1, (i+1)%256, storage_usuage, algorithm)
        elif i<(node_num-1):
            argument = "-debug -testnet -datadir=data/bcdnode%d -port=18333 -txindex=1 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=%s/0 -rpcbind=%s -addnode=%d.%d.0.1:18333 -addnode=%d.%d.0.1:18333 -storageShare=%s -algorithm=%s" % (i, (node_iphint), (node_iphint), (i-1)/256 + 1, (i-1)%256, (i+1)/256 + 1, (i+1)%256, storage_usuage, algorithm)
        else:
            argument = "-debug -testnet -datadir=data/bcdnode%d -port=18333 -txindex=1 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=%s/0 -rpcbind=%s -addnode=%d.%d.0.1:18333 -addnode=%d.%d.0.1:18333 -storageShare=%s -algorithm=%s" % (i, (node_iphint), (node_iphint), (i-1)/256 + 1, (i-1)%256, 1, 0, storage_usuage, algorithm)
        ET.SubElement(node,"application", plugin="bitcoind", time=time, arguments=argument)

    for i in range(0, node_num):
        node_id = "client%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(5)
        argument = "%d.%d.0.1:11111 %d" % (i/256 + 1, i%256, (simultime-6))
        ET.SubElement(node,"application", plugin="client", time=time, arguments=argument)

    if bool_ == True:
        tree.write(new_xml, pretty_print=True)
    else :
        node_id = "injector"
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(150)
        argument = "%d %d" % ((simultime-6), node_num)
        ET.SubElement(node,"application", plugin="txInjector", time=time, arguments=argument)

    tree.write(new_xml, pretty_print=True)

def select_option(param1,node_count, sim_time, storage_usuage, algorithm):
    if param1 == "normal":
        setup_multiple_node_xml(node_count, sim_time,True, storage_usuage, algorithm);
    elif param1 == "transaction":
        setup_multiple_node_xml(node_count, sim_time,False,storage_usuage, algorithm);



if __name__ == '__main__':

    node_count = int(sys.argv[2])
    simulation_time = int(sys.argv[3])
    storage_usuage= sys.argv[4]
    algorithm=sys.argv[5]

    select_option(sys.argv[1], node_count, simulation_time,storage_usuage,algorithm)
