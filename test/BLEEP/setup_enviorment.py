import os
import lxml.etree as ET
from subprocess import check_output
import argparse
from os.path import expanduser
import sys

def setup_multiple_node_xml(node_num):
    home = expanduser('~')
    baseline_xml = "simple-membership.xml"
    new_xml = "simple-membership-auto.xml"

    print "base xml path : " + baseline_xml
    if ~os.path.exists(baseline_xml):
        print "no such baseline file"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)
    tree   = ET.parse(baseline_xml, parser)
    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)

    for i in range(1, node_num+1):
        node_id = "bleep%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)            
        time = str(2*i)
        if i == 1:
            ET.SubElement(node,"application",plugin="plugin",time=time,arguments= node_id)
        else:
            ET.SubElement(node,"application",plugin="plugin",time=time,arguments="%s bleep1" %(node_id))
   
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

    home=expanduser('~')
    #data_dir = "%s/blockchain-sim/plugins/blockchin-peers/build"
    #if ~os.path.exists(data_dir):
    #    print "no such data directory"    
    
    shadow_configfile = "simple-membership-auto.xml"
    print "configue xml file : " + shadow_configfile
    if ~os.path.exists(shadow_configfile):
        print "no such config file"
  
    returnCode = os.system("shadow %s" % (shadow_configfile))
    if returnCode != 0 :
        print "run failed"
        sys.exit(-1)
