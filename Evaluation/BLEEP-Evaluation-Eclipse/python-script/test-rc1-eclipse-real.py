from subprocess import check_output, Popen, PIPE
import os
import argparse
import sys
import lxml.etree as ET

def setup_multiple_node_xml(node_num, simultime):
    base_xml = "xmls/real-eclipse.xml"
    new_xml  = "xmls/real-eclipse-auto.xml"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)
    tree   = ET.parse(base_xml, parser)
    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)
    for node in shadow.findall('kill'):
        shadow.remove(node)
    ET.SubElement(shadow, "kill", time=str(simultime))

    # -------------------------- Malicious node: 28 (default Active view 4 + Passive view 24) ---------------------
    for i in range(0, node_num):
        node_id = "bleep%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(i)
        if i == 0:
            #argument = "-handlenetwork=gossip -contactnode -eclipseLogTime=990"
            argument = "-handlenetwork=gossip -contactnode"
        elif i <= 28:
            eTime    = str(1000)
            argument = "-handlenetwork=gossip -eclipseTime=%s" % (eTime)
        else:
            argument = "-handlenetwork=gossip"
        
        ET.SubElement(node,"application", plugin="PEER", time=time, arguments=argument)
        
    tree.write(new_xml, pretty_print=True)

if __name__ == '__main__':
    datadir = "eclipse-datadir"
    os.system("rm -rf %s" % datadir)

    nodenum   = 100
    simultime = 0
    if len(sys.argv) > 1:
        for i, arg in enumerate(sys.argv):
            if "--nodenum" == arg:
                nodenum = int(sys.argv[i+1])
            elif "--simultime" == arg:
                simultime = int(sys.argv[i+1])
    if simultime == 0:
        simultime = 1600

    setup_multiple_node_xml(nodenum, simultime)

    os.system("./python-script/addIPHint.py xmls/internet_topology500.xml xmls/real-eclipse-auto.xml")
    shadow_configfile = "xmls/newreal-eclipse-auto.xml"
    
    shadow = Popen([os.path.expanduser("shadow"), "-d", datadir, shadow_configfile], stdout=PIPE)
    shadow_stdout = []
    while shadow.poll() is None:
        l = shadow.stdout.readline()
        #print l.strip()

    shadow_returnCode = shadow.returncode
    if shadow_returnCode != 0:
        print "test failed"
        sys.exit(-1)
