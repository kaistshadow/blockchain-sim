import sys
import lxml.etree as ET

def setup_multiple_node_xml(node_num, simultime):

    base_xml = "example_pos.xml"
    new_xml  = "output.xml"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)
    tree   = ET.parse(base_xml, parser)
    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)
    for node in shadow.findall('kill'):
        shadow.remove(node)
    ET.SubElement(shadow, "kill", time=str(simultime))

    for i in range(0, node_num):
        node_id = "posnode%d" % (i)
        node = ET.SubElement(shadow, "node", id=node_id)
        time = str(0)

        argument = "-id=posnode%d -stakefile=stakefile.txt -slotinterval=30 -creatorid=%d" % (i, (i+1))
        argument += " -connect=posnode%d" % ((i + (4 - 1)) / 4 - 1)

        ET.SubElement(node,"application", plugin="NODE", time=time, arguments=argument)

    tree.write(new_xml, pretty_print=True)
    print("generating xml is finished ")
