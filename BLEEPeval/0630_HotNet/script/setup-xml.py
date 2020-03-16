import argparse
import sys
import lxml.etree as ET

def setup_pow_basicnet(node_num, simultime, miningtime, miningtime_dev):
    baseline_xml = "config-examples/rc1-eventloop.xml"
    new_xml = "rc1-pow-%snode.xml" % str(node_num)

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)

    tree = ET.parse(baseline_xml, parser)

    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)

    for node in shadow.findall('kill'):
        shadow.remove(node)

    for node in shadow.findall('plugin'):
        shadow.remove(node)

    ET.SubElement(shadow, "plugin", id="NODE", path="libNODE_POW.so")
    ET.SubElement(shadow, "plugin", id="CLIENT", path="libNODE_client.so")

    ET.SubElement(shadow, "kill", time=str(simultime))

    # node = ET.SubElement(shadow, "node", id="bleep0")
    # ET.SubElement(node, "application", plugin="NODE", time="0", arguments="-proxynode")


    for i in range(0,node_num):
        connectargs = ""
        for j in range(0, node_num):
            connectargs += "-connect=bleep%d " % j
            

        node_id = "bleep%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)
        # time = str(5 + i/100)
        time = str(0)
        argument = "-id=%s -l=debug -miningtime=%d -miningtimedev=%s %s" % (node_id, miningtime, miningtime_dev, connectargs)
        ET.SubElement(node, "application", plugin="NODE", time=time, arguments=argument)


    client = ET.SubElement(shadow, "node", id="client0")
    connectargs = ""
    for i in range(0, node_num):
        connectargs += "-connect=bleep%d " % i
    argument = "-id=client0 -txgeninterval=1 %s" % connectargs
    ET.SubElement(client, "application", plugin="CLIENT", time="5", arguments=argument)
    # node_id = "bleep%d" % (node_num)
    # node = ET.SubElement(shadow, "node", id=node_id)
    # ET.SubElement(node, "application", plugin="PEER", time="10", arguments="-networkparticipant -generatetx=%d -timegeneratetx=%d" % (txnum, txnum/100))

    tree.write(new_xml, pretty_print=True)

def setup_pow_random(node_num, simultime, miningtime, miningtime_dev):
    baseline_xml = "config-examples/rc1-eventloop.xml"
    new_xml = "rc1-pow-random-%snode.xml" % str(node_num)

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)

    tree = ET.parse(baseline_xml, parser)

    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)

    for node in shadow.findall('kill'):
        shadow.remove(node)

    for node in shadow.findall('plugin'):
        shadow.remove(node)

    ET.SubElement(shadow, "plugin", id="NODE", path="libNODE_POW_RANDOM.so")
    ET.SubElement(shadow, "plugin", id="CLIENT", path="libNODE_client.so")

    ET.SubElement(shadow, "kill", time=str(simultime))

    # node = ET.SubElement(shadow, "node", id="bleep0")
    # ET.SubElement(node, "application", plugin="NODE", time="0", arguments="-proxynode")


    for i in range(0,node_num):
        connectargs = ""
        for j in range(0, node_num):
            connectargs += "-connect=bleep%d " % j
            

        node_id = "bleep%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)
        # time = str(5 + i/100)
        time = str(0)
        argument = "-txgeninterval=1000000000 -outpeernum=20 -fanout=5"
        if i == 100:
            argument += " -id=%s -l=debug -miningtime=%d -miningtimedev=%s %s" % (node_id, miningtime, miningtime_dev, connectargs)
        else:
            argument += " -id=%s -l=debug -miningtime=%d -miningtimedev=%s %s" % (node_id, 1000, miningtime_dev, connectargs)
        ET.SubElement(node, "application", plugin="NODE", time=time, arguments=argument)


    client = ET.SubElement(shadow, "node", id="client0")
    # for i in range(0, node_num):
    #     connectargs += "-connect=bleep%d " % i
    connectargs = "-connect=bleep100"

    argument = "-id=client0 -txgeninterval=1 %s" % connectargs
    ET.SubElement(client, "application", plugin="CLIENT", time="5", arguments=argument)
    # node_id = "bleep%d" % (node_num)
    # node = ET.SubElement(shadow, "node", id=node_id)
    # ET.SubElement(node, "application", plugin="PEER", time="10", arguments="-networkparticipant -generatetx=%d -timegeneratetx=%d" % (txnum, txnum/100))

    tree.write(new_xml, pretty_print=True)


def setup_pow_random_with_spanningtree_smalltest(node_num, simultime, miningtime, miningtime_dev):
    baseline_xml = "config-examples/rc1-eventloop.xml"
    new_xml = "rc1-pow-random-st-%snode.xml" % str(node_num)

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)

    tree = ET.parse(baseline_xml, parser)

    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)

    for node in shadow.findall('kill'):
        shadow.remove(node)

    for node in shadow.findall('plugin'):
        shadow.remove(node)

    ET.SubElement(shadow, "plugin", id="NODE", path="libNODE_POW_RANDOM.so")
    ET.SubElement(shadow, "plugin", id="CLIENT", path="libNODE_client.so")

    ET.SubElement(shadow, "kill", time=str(simultime))

    # node = ET.SubElement(shadow, "node", id="bleep0")
    # ET.SubElement(node, "application", plugin="NODE", time="0", arguments="-proxynode")

    childnum = 0
    for i in range(2,30):
        if (i * i * i) + (i*i) + i + 1 >= node_num:
            childnum = i
            break


    for i in range(0,node_num):
        connectargs = ""
        if i <= childnum + childnum*childnum:
            for j in range(i*childnum + 1, (i+1)*childnum +1):
                if j < node_num:
                    connectargs += "-connect=bleep%d " % j

        # connectargs = ""
        # for j in range(0, node_num):
        #     connectargs += "-connect=bleep%d " % j
            

        node_id = "bleep%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)
        # time = str(5 + i/100)
        time = str(0)
        argument = "-txgeninterval=1000000000 -outpeernum=20 -fanout=%d" % (childnum+1)
        if i == 100:
            argument += " -id=%s -l=debug -miningtime=%d -miningtimedev=%s %s" % (node_id, miningtime, miningtime_dev, connectargs)
        else:
            argument += " -id=%s -l=debug -miningtime=%d -miningtimedev=%s %s" % (node_id, 1000, miningtime_dev, connectargs)
        ET.SubElement(node, "application", plugin="NODE", time=time, arguments=argument)


    client = ET.SubElement(shadow, "node", id="client0")
    # for i in range(0, node_num):
    #     connectargs += "-connect=bleep%d " % i
    connectargs = "-connect=bleep100"

    argument = "-id=client0 -txgeninterval=1 %s" % connectargs
    ET.SubElement(client, "application", plugin="CLIENT", time="5", arguments=argument)
    # node_id = "bleep%d" % (node_num)
    # node = ET.SubElement(shadow, "node", id=node_id)
    # ET.SubElement(node, "application", plugin="PEER", time="10", arguments="-networkparticipant -generatetx=%d -timegeneratetx=%d" % (txnum, txnum/100))

    tree.write(new_xml, pretty_print=True)

def setup_pow_random_with_spanningtree(node_num, simultime, miningtime, miningtime_dev):
    baseline_xml = "config-examples/rc1-eventloop.xml"
    new_xml = "rc1-pow-random-st-%snode.xml" % str(node_num)

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)

    tree = ET.parse(baseline_xml, parser)

    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)

    for node in shadow.findall('kill'):
        shadow.remove(node)

    for node in shadow.findall('plugin'):
        shadow.remove(node)

    ET.SubElement(shadow, "plugin", id="NODE", path="libNODE_POW_RANDOM.so")
    ET.SubElement(shadow, "plugin", id="CLIENT", path="libNODE_client.so")

    ET.SubElement(shadow, "kill", time=str(simultime))

    # node = ET.SubElement(shadow, "node", id="bleep0")
    # ET.SubElement(node, "application", plugin="NODE", time="0", arguments="-proxynode")

    childnum = 0
    for i in range(2,30):
        if (i * i * i) + (i*i) + i + 1 >= node_num:
            childnum = i
            break


    for i in range(0,node_num):
        connectargs = ""
        if i <= childnum + childnum*childnum:
            for j in range(i*childnum + 1, (i+1)*childnum +1):
                if j < node_num:
                    connectargs += "-connect=bleep%d " % j

        # connectargs = ""
        # for j in range(0, node_num):
        #     connectargs += "-connect=bleep%d " % j
            

        node_id = "bleep%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)
        # time = str(5 + i/100)
        time = str(0)
        argument = "-txgeninterval=1000000000 -outpeernum=20 -fanout=%d" % (childnum+1)
        argument += " -id=%s -l=debug -miningtime=%d -miningtimedev=%s %s" % (node_id, miningtime, miningtime_dev, connectargs)
        ET.SubElement(node, "application", plugin="NODE", time=time, arguments=argument)


    client = ET.SubElement(shadow, "node", id="client0")
    for i in range(0, node_num):
        connectargs += "-connect=bleep%d " % i

    argument = "-id=client0 -txgeninterval=1 %s" % connectargs
    ET.SubElement(client, "application", plugin="CLIENT", time="5", arguments=argument)
    # node_id = "bleep%d" % (node_num)
    # node = ET.SubElement(shadow, "node", id=node_id)
    # ET.SubElement(node, "application", plugin="PEER", time="10", arguments="-networkparticipant -generatetx=%d -timegeneratetx=%d" % (txnum, txnum/100))

    tree.write(new_xml, pretty_print=True)


def setup_pow_random_tree_with_spanningtree(node_num, simultime, miningtime, miningtime_dev):
    baseline_xml = "config-examples/rc1-eventloop.xml"
    new_xml = "rc1-pow-random-tree-st-%snode.xml" % str(node_num)

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)

    tree = ET.parse(baseline_xml, parser)

    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)

    for node in shadow.findall('kill'):
        shadow.remove(node)

    for node in shadow.findall('plugin'):
        shadow.remove(node)

    ET.SubElement(shadow, "plugin", id="NODE", path="libNODE_POW_RANDOM_TREE.so")
    ET.SubElement(shadow, "plugin", id="CLIENT", path="libNODE_client.so")

    ET.SubElement(shadow, "kill", time=str(simultime))

    # node = ET.SubElement(shadow, "node", id="bleep0")
    # ET.SubElement(node, "application", plugin="NODE", time="0", arguments="-proxynode")

    childnum = 0
    for i in range(2,30):
        if (i * i * i) + (i*i) + i + 1 >= node_num:
            childnum = i
            break


    for i in range(0,node_num):
        connectargs = ""
        if i <= childnum + childnum*childnum:
            for j in range(i*childnum + 1, (i+1)*childnum +1):
                if j < node_num:
                    connectargs += "-connect=bleep%d " % j

        # connectargs = ""
        # for j in range(0, node_num):
        #     connectargs += "-connect=bleep%d " % j
            

        node_id = "bleep%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)
        # time = str(5 + i/100)
        time = str(0)
        argument = "-txgeninterval=1000000000 -outpeernum=20 -fanout=%d" % (childnum+1)
        argument += " -id=%s -l=debug -miningtime=%d -miningtimedev=%s %s" % (node_id, miningtime, miningtime_dev, connectargs)
        ET.SubElement(node, "application", plugin="NODE", time=time, arguments=argument)


    client = ET.SubElement(shadow, "node", id="client0")
    for i in range(0, node_num):
        connectargs += "-connect=bleep%d " % i

    argument = "-id=client0 -txgeninterval=1 %s" % connectargs
    ET.SubElement(client, "application", plugin="CLIENT", time="5", arguments=argument)
    # node_id = "bleep%d" % (node_num)
    # node = ET.SubElement(shadow, "node", id=node_id)
    # ET.SubElement(node, "application", plugin="PEER", time="10", arguments="-networkparticipant -generatetx=%d -timegeneratetx=%d" % (txnum, txnum/100))

    tree.write(new_xml, pretty_print=True)



def setup_multiple_node_xml_centralized(node_num, simultime, miningtime, miningtime_dev):
    baseline_xml = "config-examples/rc1-eventloop.xml"
    new_xml = "rc1-rtt-pow-ran-1000node.xml"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)

    tree = ET.parse(baseline_xml, parser)

    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)

    for node in shadow.findall('kill'):
        shadow.remove(node)

    for node in shadow.findall('plugin'):
        shadow.remove(node)

    ET.SubElement(shadow, "plugin", id="NODE", path="libNODE_POW_RANDOM_TREE.so")

    ET.SubElement(shadow, "kill", time=str(simultime))


    # node = ET.SubElement(shadow, "node", id="bleep0")
    # ET.SubElement(node, "application", plugin="NODE", time="0", arguments="-proxynode")


    for i in range(0,node_num):
        connectargs = ""
        for j in range(0, node_num):
            connectargs += "-connect=bleep%d " % j

        node_id = "bleep%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)
        # time = str(5 + i/100)
        time = str(0)
        argument = "-id=%s -fanout=300 -outpeernum=300 -miningtime=%d -miningtimedev=%s -txgeninterval=1 %s" % (node_id, miningtime, miningtime_dev, connectargs)
        ET.SubElement(node, "application", plugin="NODE", time=time, arguments=argument)

    # node_id = "bleep%d" % (node_num)
    # node = ET.SubElement(shadow, "node", id=node_id)
    # ET.SubElement(node, "application", plugin="PEER", time="10", arguments="-networkparticipant -generatetx=%d -timegeneratetx=%d" % (txnum, txnum/100))

    tree.write(new_xml, pretty_print=True)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--t", help="Type of generated configuration : 'b' (pow+basic), 'r' (pow+random), 'rt' (pow+random with spanning tree), 'ptst' (pow(tree)+random with spanning tree) [default:'b']")

    
    args = parser.parse_args()
    if not args.t:
        exptype = "b"
    else:
        exptype = args.t

    nodenum = 200
    simultime = 100
    miningtime = 10
    miningtimedev = "3"

    # nodenum = 1000
    # simultime = 1200
    # miningtime = 15
    # miningtimedev = "3"

    # nodenum = 1000
    # simultime = 1200
    # miningtime = 10
    # miningtimedev = "0.5"

    # setup_multiple_node_xml_centralized(nodenum, simultime, miningtime, miningtimedev)

    if exptype == "b":
        setup_pow_basicnet(nodenum, simultime, miningtime, miningtimedev)
    elif exptype == "r":
        setup_pow_random(nodenum, simultime, miningtime, miningtimedev)
    elif exptype == "rt":
        setup_pow_random_with_spanningtree(nodenum, simultime, miningtime, miningtimedev)
    elif exptype == "ptst":
        setup_pow_random_tree_with_spanningtree(nodenum, simultime, miningtime, miningtimedev)
