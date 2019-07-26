import argparse
import sys
import lxml.etree as ET

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

    ET.SubElement(shadow, "plugin", id="NODE", path="~/.bleep/plugins/libNODE_RTT_POW_RAN.so")

    ET.SubElement(shadow, "kill", time=str(simultime))


    # node = ET.SubElement(shadow, "node", id="bleep0")
    # ET.SubElement(node, "application", plugin="NODE", time="0", arguments="-proxynode")


    for i in range(0,node_num):
        connectargs = ""
        for j in range(0, i):
            connectargs += "-connect=bleep%d " % j

        node_id = "bleep%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)
        # time = str(5 + i/100)
        time = str(0)
        argument = "-id=%s -fanout=20 -outpeernum=40 -miningtime=%d -miningtimedev=%s %s" % (node_id, miningtime, miningtime_dev, connectargs)
        ET.SubElement(node, "application", plugin="NODE", time=time, arguments=argument)

    # node_id = "bleep%d" % (node_num)
    # node = ET.SubElement(shadow, "node", id=node_id)
    # ET.SubElement(node, "application", plugin="PEER", time="10", arguments="-networkparticipant -generatetx=%d -timegeneratetx=%d" % (txnum, txnum/100))

    tree.write(new_xml, pretty_print=True)


if __name__ == '__main__':

    nodenum = 1000
    simultime = 11000
    miningtime = 100
    miningtimedev = "3"

    # nodenum = 1000
    # simultime = 1200
    # miningtime = 15
    # miningtimedev = "3"

    # nodenum = 1000
    # simultime = 1200
    # miningtime = 10
    # miningtimedev = "0.5"

    setup_multiple_node_xml_centralized(nodenum, simultime, miningtime, miningtimedev)

