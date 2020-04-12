import os
from subprocess import check_output, Popen, PIPE
import argparse
import sys
import lxml.etree as ET

def setup_multiple_node_xml(node_num, injector_op, simultime, txnum, miningtime, miningtime_dev):
    base_xml = "rc1.xml"
    new_xml  = "rc1-gossip-auto.xml"

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
        time = str(i)
        if i == 0:
            argument = "-handlenetwork=gossip -contactnode -miningtime=%d -miningtimedev=%s" % (miningtime, miningtime_dev)
        else:
            argument = "-handlenetwork=gossip -miningtime=%d -miningtimedev=%s" % (miningtime, miningtime_dev)
        ET.SubElement(node,"application", plugin="PEER", time=time, arguments=argument)

    if injector_op and txnum > 0:

        # ----------------------------------- msg for building gossip overlay ----------------------------------- 
        node_id = "bleep%d" % node_num
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+200),\
                    arguments="-handlenetwork=gossip -contact=bleep0 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)


        # ----------------------------------- msg broadcasting : 10 injector  ----------------------------------- 
        node_id = "bleep%d" % (node_num+1)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+300),\
                    arguments="-handlenetwork=gossip -contact=bleep0 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

        node_id = "bleep%d" % (node_num+2)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+350),\
                    arguments="-handlenetwork=gossip -contact=bleep50 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

        node_id = "bleep%d" % (node_num+3)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+400),\
                    arguments="-handlenetwork=gossip -contact=bleep100 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

        node_id = "bleep%d" % (node_num+4)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+450),\
                    arguments="-handlenetwork=gossip -contact=bleep150 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

        node_id = "bleep%d" % (node_num+5)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+500),\
                    arguments="-handlenetwork=gossip -contact=bleep200 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

        node_id = "bleep%d" % (node_num+6)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+550),\
                    arguments="-handlenetwork=gossip -contact=bleep250 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

        node_id = "bleep%d" % (node_num+7)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+600),\
                    arguments="-handlenetwork=gossip -contact=bleep300 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

        node_id = "bleep%d" % (node_num+8)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+650),\
                    arguments="-handlenetwork=gossip -contact=bleep350 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

        node_id = "bleep%d" % (node_num+9)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+700),\
                    arguments="-handlenetwork=gossip -contact=bleep400 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

        node_id = "bleep%d" % (node_num+10)
        node = ET.SubElement(shadow, "node", id=node_id)
        ET.SubElement(node, "application", plugin="PEER", time=str(node_num+750),\
                    arguments="-handlenetwork=gossip -contact=bleep450 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)
        
    tree.write(new_xml, pretty_print=True)


if __name__ == '__main__':
    datadir = "rc1-datadir"
    shadow_configfile = "rc1-gossip-auto.xml"
    os.system("rm -rf %s" % datadir)

    injector_op = False
    nodenum     = 100
    simultime   = nodenum+400
    txnum       = 1
    miningtime  = 10
    miningtimedev = "3.0"
    if len(sys.argv) > 1:
        for i, arg in enumerate(sys.argv):
            if "-injector" == arg:
                injector_op = True
            elif "--nodenum" == arg:
                nodenum = int(sys.argv[i+1])
            elif "--simultime" == arg:
                simultime = int(sys.argv[i+1])
            elif "--txnum" == arg:
                txnum = int(sys.argv[i+1])
            elif "--miningtime" == arg:
                miningtime = int(sys.argv[i+1])
            elif "--miningtimedev" == arg:
                miningtimedev = sys.argv[i+1]



                
    simultime = nodenum+800
    setup_multiple_node_xml(nodenum, injector_op, simultime, txnum, miningtime, miningtimedev)


    
    shadow = Popen([os.path.expanduser("shadow"), "-d", datadir, shadow_configfile], stdout=PIPE)
    shadow_stdout = []
    while shadow.poll() is None:
        l = shadow.stdout.readline()
        print l.strip()
        #log = l.strip()
        #print log
        #shadow_stdout.append(log)

    shadow_returnCode = shadow.returncode
    '''
    for line in shadow_stdout:
        if "critical" in line:
            print line
            print "critical error is occurred during shadow simulation"
            sys.exit(-1)
    '''
    if shadow_returnCode != 0:
        print "test failed"
        sys.exit(-1)
