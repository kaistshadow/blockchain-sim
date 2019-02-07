import os
from subprocess import check_output, Popen, PIPE
import argparse
import sys
import lxml.etree as ET

def setup_multiple_node_xml(node_num, injector_op):
    baseline_xml = "rc1.xml"
    new_xml = "rc1-gossip-auto.xml"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)
    tree = ET.parse(baseline_xml, parser)
    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)
    for node in shadow.findall('kill'):
        shadow.remove(node)
    ET.SubElement(shadow, "kill", time=str(node_num+200))

    for i in range(0, node_num):
        node_id = "bleep%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(i)
        if i == 0:
            argument = "-handlenetwork=gossip -contactnode"
        else:
            argument = "-handlenetwork=gossip"
        ET.SubElement(node,"application", plugin="PEER", time=time, arguments=argument)

    if injector_op:
        node_id = "bleep%d" % node_num
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+100), arguments="-handlenetwork=gossip -networkparticipant -generatetx=4 -timegeneratetx=1")

    tree.write(new_xml, pretty_print=True)

if __name__ == '__main__':
    nodenum = 0
    injector_op = False
    for i, arg in enumerate(sys.argv):
        if "--nodenum" == arg:
            nodenum = int(sys.argv[i+1])
        if "-injector" == arg:
            injector_op = True

    setup_multiple_node_xml(nodenum, injector_op)
    datadir = "rc1-gossip-datadir"
    shadow_configfile = "rc1-gossip-auto.xml"

    shadow = Popen([os.path.expanduser("shadow"), "-d", datadir, "-l", "message", "--cpu-threshold=-1", shadow_configfile], stdout=PIPE)

    shadow_stdout = []
    while shadow.poll() is None:
        l = shadow.stdout.readline()
        print l.strip()
        shadow_stdout.append(l.strip())

    shadow_returnCode = shadow.returncode
    for line in shadow_stdout:
        if "critical" in shadow_stdout:
            print "critical error is occurred during shadow simulation"
            sys.exit(-1)

    if shadow_returnCode != 0:
        print "test failed"
        sys.exit(-1)

    test_pass = True
