import os
import lxml.etree as ET
from subprocess import check_output
import argparse
from os.path import expanduser
import sys

def setup_multiple_node_xml(node_num):
    home = expanduser('~')
    baseline_xml = "10node-txgossip.xml"
    new_xml = "%dnode-txgossip.xml" % node_num

    print "base xml path : " + baseline_xml
    if ~os.path.exists(baseline_xml):
        print "no such baseline file"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)
    tree   = ET.parse(baseline_xml, parser)
    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)

    node1 = ET.SubElement(shadow, "node", id="bleep1")
    ET.SubElement(node1, "application", plugin="NODE", time="5", arguments="-id=bleep1")

    for i in range(2, node_num+1):
        node_id = "bleep%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(i + 5)
        parent_id = "bleep%d" % int(i/2)

        ET.SubElement(node,"application",plugin="NODE",time=time,arguments="-id=%s -connect=%s" % (node_id, parent_id))


    tree.write(new_xml, pretty_print=True)
    print "new xml path : " + new_xml
    return 0

if __name__ == '__main__':
    if len (sys.argv) != 2 :
        print "Command line input error %d" % len(sys.argv)
        sys.exit (-1)

    num = int(sys.argv[1])
    print "num: %d" % num

    res = setup_multiple_node_xml(num)
    if res == -1:
        print "setup xml fail"
        sys.exit(-1)


