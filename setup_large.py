import lxml.etree as ET

def setup_multiple_node_xml(node_num):
    baseline_xml = "test/BLEEP/pow-consensus.xml"
    new_xml = "test/BLEEP/pow-consensus.large.xml"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)

    tree = ET.parse(baseline_xml, parser)

    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)
        
    for node in shadow.findall('kill'):
        shadow.remove(node)
    ET.SubElement(shadow, "kill", time="50")


    node = ET.SubElement(shadow, "node", id="bleep0")
    ET.SubElement(node, "application", plugin="DUMP_POWBLOCKCHAIN", time="3", arguments="")
    
    for i in range(1,node_num):
        node_id = "bleep%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)            
        time = str(5 + i/100)
        if i == 1:
            argument = "bleep1"
        else:
            argument = "bleep%d bleep1" % i
        ET.SubElement(node, "application", plugin="PEER_POWCONSENSUS", time=time, arguments=argument)

    node_id = "bleep%d" % (node_num)
    node = ET.SubElement(shadow, "node", id=node_id)
    ET.SubElement(node, "application", plugin="INJECTOR_POWCONSENSUS", time="10", arguments="bleep1 5")

    tree.write(new_xml, pretty_print=True)

if __name__ == '__main__':
    setup_multiple_node_xml(10)
