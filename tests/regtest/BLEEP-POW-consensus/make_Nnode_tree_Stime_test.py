import os
from subprocess import check_output, Popen, PIPE
import argparse
import sys
import lxml.etree as ET

def setup_multiple_node_xml(node_num, client_num, simultime, txgeninterval, block_txnum, miningtime):
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
        node_id = "bleep%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(0)
        argument = "-txgeninterval=1000000000 -outpeernum=20 -fanout=7 -blocktxnum=%d -id=bleep%d -l=debug -miningtime=%d -miningnodecnt=%d" % (block_txnum, i, miningtime, node_num)
        if(node_num > 1):
            if(i < ((node_num-1)//6)):
                argument += " -connect=bleep%d" % ((i*6+1))
                argument += " -connect=bleep%d" % ((i*6+2))
                argument += " -connect=bleep%d" % ((i*6+3))
                argument += " -connect=bleep%d" % ((i*6+4))
                argument += " -connect=bleep%d" % ((i*6+5))
                argument += " -connect=bleep%d" % ((i*6+6))
            elif(i == ((node_num-1)//6)):
                for j in range((node_num-1)%6):
                    argument += " -connect=bleep%d" % ((i*6+j+1))

        ET.SubElement(node,"application", plugin="NODE", time=time, arguments=argument)

    for i in range(0, client_num):
        node_id = "client%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(5)
        argument = "-id=client%d -txgeninterval=%d" % (i, txgeninterval)
        for j in range(0, node_num):
            argument += " -connect=bleep%d" % j
        ET.SubElement(node,"application", plugin="CLIENT", time=time, arguments=argument)
    tree.write(new_xml, pretty_print=True)

if __name__ == '__main__':
    node_count = int(sys.argv[1])
    block_txnum = 4000
    mining_time = 600
    client_count = (block_txnum//mining_time) + 1
    simulation_time = int(sys.argv[2])
    txgen_interval = 1

    shadow_configfile = "base"+str(node_count)+"N"+str(simulation_time)+"T.xml"

    setup_multiple_node_xml(node_count, client_count, simulation_time, txgen_interval, block_txnum, mining_time)