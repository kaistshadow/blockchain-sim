import sys
import lxml.etree as ET

def setup_multiple_node_xml(node_num, simultime, bool_, algorithm, difficulty):

    base_xml = "example.xml"
    new_xml  = "output.xml"

    if bool_ == True:
        new_xml = sys.argv[6] + "/" + new_xml
    else:
        new_xml = sys.argv[9] + "/" + new_xml


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
            argument = "-debug -datadir=data/bcdnode%d -port=18333 -txindex=1 -fallbackfee=0.0002 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=%s/0 -rpcbind=%s -addnode=%d.%d.0.1:18333 -addnode=%d.%d.0.1:18333 -algorithm=%s -difficulty=%s" % (i, (node_iphint), (node_iphint), (node_num-1)/256 + 1, (node_num-1)%256, (i+1)/256 + 1, (i+1)%256, algorithm, difficulty)
        elif i<(node_num-1):
            argument = "-debug -datadir=data/bcdnode%d -port=18333 -txindex=1 -fallbackfee=0.0002 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=%s/0 -rpcbind=%s -addnode=%d.%d.0.1:18333 -addnode=%d.%d.0.1:18333 -algorithm=%s -difficulty=%s" % (i, (node_iphint), (node_iphint), (i-1)/256 + 1, (i-1)%256, (i+1)/256 + 1, (i+1)%256, algorithm, difficulty)
        else:
            argument = "-debug -datadir=data/bcdnode%d -port=18333 -txindex=1 -fallbackfee=0.0002 -rpcuser=a -rpcpassword=1234 -rpcport=11111 -rpcallowip=%s/0 -rpcbind=%s -addnode=%d.%d.0.1:18333 -addnode=%d.%d.0.1:18333 -algorithm=%s -difficulty=%s" % (i, (node_iphint), (node_iphint), (i-1)/256 + 1, (i-1)%256, 1, 0, algorithm, difficulty)
        ET.SubElement(node,"application", plugin="bitcoind", time=time, arguments=argument)

    for i in range(0, node_num):
        node_id = "client%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(5)
        argument = "%d.%d.0.1:11111 %d " % (i/256 + 1, i%256, (simultime-7))
        ET.SubElement(node,"application", plugin="client", time=time, arguments=argument)

    if bool_ == True:
        
        tree.write(new_xml, pretty_print=True)
    else :
        node_id = "injector"
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(5)
        txcnt = sys.argv[6]
        if txcnt > -1:
            txsec = sys.argv[7]
            amount = sys.argv[8]
            argument = " %d.%d.0.1:11111 %s %s %s " % (i % 256 + 1, i % 256, txcnt, txsec, amount)
        elif txcnt == "-1 ":
             argument= " %d.%d.0.1:11111 0 0 0 " %(i/256 + 1, i%256)
        ET.SubElement(node,"application", plugin="txInjector", time=time, arguments=argument)

    tree.write(new_xml, pretty_print=True)
    print("generating xml is finished ")

def select_option(param1,node_count, sim_time, algorithm, difficulty):
    if param1 == "disable":
        setup_multiple_node_xml(node_count, sim_time, True, algorithm, difficulty)
    elif param1 == "enable":
        print("enable is start  ")
        setup_multiple_node_xml(node_count, sim_time, False, algorithm, difficulty)

if __name__ == '__main__':
    print("start make_approximate_setmining_test.py start ")

    node_count = int(sys.argv[1])
    simulation_time = int(sys.argv[2])
    algorithm = sys.argv[3]
    txmode = sys.argv[4]
    difficulty = sys.argv[5]
    
    select_option(txmode, node_count, simulation_time, algorithm, difficulty)