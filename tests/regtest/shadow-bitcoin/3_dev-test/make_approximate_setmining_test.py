import os
from subprocess import check_output, Popen, PIPE
import argparse
import sys
import lxml.etree as ET

def setup_multiple_node_xml(node_num, simultime):
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
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(0)
        if i==0:
            argument = "-debug -testnet -datadir=data/bcdnode%d -port=11110 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=11.0.0.%d/0 -rpcbind=11.0.0.%d -addnode=11.0.0.%d:11110 -addnode=11.0.0.%d:11110" % (i, (i+1), (i+1), node_num, (i+2))
        else:
            argument = "-debug -testnet -datadir=data/bcdnode%d -port=11110 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=11.0.0.%d/0 -rpcbind=11.0.0.%d -addnode=11.0.0.%d:11110 -addnode=11.0.0.%d:11110" % (i, (i+1), (i+1), i, (i+2))
        ET.SubElement(node,"application", plugin="NODE", time=time, arguments=argument)

    for i in range(0, node_num):
        node_id = "client%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(5)
        argument = "11.0.0.%d:11111 %d" % ((i+1), (simultime-6))
        ET.SubElement(node,"application", plugin="client", time=time, arguments=argument)
    tree.write(new_xml, pretty_print=True)


if __name__ == '__main__':

    node_count = int(sys.argv[1])
    simulation_time = int(sys.argv[2])

    setup_multiple_node_xml(node_count, simulation_time)
