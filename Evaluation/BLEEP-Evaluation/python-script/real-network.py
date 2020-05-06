from subprocess import check_output, Popen, PIPE
import os
import argparse
import sys
import lxml.etree as ET

def setup_multiple_node_xml(node_num, injector_op, simultime, txnum, miningtime, miningtime_dev):
    base_xml = "xmls/real-network.xml"
    new_xml  = "xmls/real-network-auto.xml"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)
    tree   = ET.parse(base_xml, parser)
    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)
    for node in shadow.findall('kill'):
        shadow.remove(node)
    ET.SubElement(shadow, "kill", time=str(simultime))

    # ----------------------------------- Generate node and add them to xml file --------------------------- 
    # Set id 0 as contact node that every other new nodes can enter this network by contacting to the contact node
    for i in range(0, node_num):
        node_id = "bleep%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(i)
        if i == 0:
            argument = "-handlenetwork=gossip -contactnode -miningtime=%d -miningtimedev=%s" % (miningtime, miningtime_dev)
        else:
            argument = "-handlenetwork=gossip -miningtime=%d -miningtimedev=%s" % (miningtime, miningtime_dev)
        ET.SubElement(node,"application", plugin="PEER", time=time, arguments=argument)

    if not injector_op or txnum <= 0:    
        tree.write(new_xml, pretty_print=True)
        return

    # ----------------------------------- msg for building gossip overlay ----------------------------------- 
    # Since gossip overlay scatter after injecting the first tx, it's needed to wait for stabiliaztion
    # Time required for stablization can be checked via non-injection case (only gossip nodes)
    node_id = "bleep%d" % node_num
    node = ET.SubElement(shadow, "node", id=node_id)
    ET.SubElement(node, "application", plugin="PEER", time=str(node_num+350),\
                arguments="-handlenetwork=gossip -contact=bleep0 -networkparticipant -generatetx=%d -timegeneratetx=1" %txnum)

    # ----------------------------------- msg broadcasting : 10 injector (default) --------------------------
    # After gossip network stablization (Tree established), choose several nodes that receive a tx from injectors
    # Injected txs has own msg id so that we can calculate propagation delay (from injection time to time that last node receive the tx)
    num_injector = 10
    for i in range(0, num_injector):
        # Injector = addtional node id (not join P2P network)
        injector_id = "bleep%d" % (node_num+1+i)             
        # Uniformly distributed target node (target node = receive tx from injector)
        # There are many other ways to choose injected node
        targetnode  = "bleep%d" % ((node_num/num_injector)*i)
        time = str(node_num+500+50*i) 
        print "[Injector]", injector_id, "->", targetnode, ", injection time:",time

        node = ET.SubElement(shadow, "node", id=injector_id)
        ET.SubElement(node, "application", plugin="PEER", time=time,\
                    arguments="-handlenetwork=gossip -contact=%s -networkparticipant -generatetx=%d -timegeneratetx=1" % (targetnode, txnum))
    
    tree.write(new_xml, pretty_print=True)


if __name__ == '__main__':
    datadir = "rc1-datadir"
    os.system("rm -rf %s" % datadir)

    injector_op = False
    nodenum     = 100
    simultime   = nodenum+400
    txnum       = 1
    miningtime  = 10
    miningtimedev = "3.0"
    if len(sys.argv) > 1:
        for i, arg in enumerate(sys.argv):
            if "--injector" == arg:
                injector_op = True
                simultime   = nodenum+1500 # can be changed. depends on # of injector (default 10 -> 950 secs)
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

    simultime = nodenum+1000
    setup_multiple_node_xml(nodenum, injector_op, simultime, txnum, miningtime, miningtimedev)

    shadow_configfile = "xmls/real-network-auto.xml"
    os.system("./python-script/addIPHint.py xmls/internet_topology500.xml xmls/real-network-auto.xml")
    
    shadow_configfile = "xmls/newreal-network-auto.xml"
    
    shadow = Popen([os.path.expanduser("shadow"), "-d", datadir, shadow_configfile], stdout=PIPE)
    shadow_stdout = []
    while shadow.poll() is None:
        l = shadow.stdout.readline()
        #print l.strip()

    shadow_returnCode = shadow.returncode

    if shadow_returnCode != 0:
        print "test failed"
        sys.exit(-1)
