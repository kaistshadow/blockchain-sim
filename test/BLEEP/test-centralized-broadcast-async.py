import os
from subprocess import check_output, Popen, PIPE
import argparse
import sys
import lxml.etree as ET

def setup_multiple_node_xml_centralized(node_num):
    baseline_xml = "centralized-broadcast-async.xml"
    new_xml = "centralized-broadcast-async-large.xml"

    parser = ET.XMLParser(remove_blank_text=True, strip_cdata=False)

    tree = ET.parse(baseline_xml, parser)

    shadow = tree.getroot()

    for node in shadow.findall('node'):
        shadow.remove(node)
        
    for node in shadow.findall('kill'):
        shadow.remove(node)
    ET.SubElement(shadow, "kill", time="150")


    node = ET.SubElement(shadow, "node", id="dump_blockchain")
    ET.SubElement(node, "application", plugin="DUMP_POWBLOCKCHAIN", time="2", arguments="")


    # node_array = ["bleep%d"% i for i in range(1,node_num)]
    node = ET.SubElement(shadow, "node", id="bleep0")
    ET.SubElement(node, "application", plugin="PROXY_POWCONSENSUS", time="3", arguments="bleep0")
    # ET.SubElement(node, "application", plugin="PROXY_POWCONSENSUS", time="3", arguments="bleep0 "+ " ".join(node_array))
    
    for i in range(1,node_num):
        node_id = "bleep%d" % (i)

        node = ET.SubElement(shadow, "node", id=node_id)            
        # time = str(5 + i/100)
        time = str(5)
        argument = "bleep1"
        argument = "bleep%d bleep0" % i
        ET.SubElement(node, "application", plugin="PEER_POWCONSENSUS", time=time, arguments=argument)

    node_id = "bleep%d" % (node_num)
    node = ET.SubElement(shadow, "node", id=node_id)
    ET.SubElement(node, "application", plugin="INJECTOR_POWCONSENSUS", time="10", arguments="bleep0 30")

    tree.write(new_xml, pretty_print=True)


if __name__ == '__main__':
    datadir = "centralized-broadcast-async-datadir"
    shadow_configfile = "centralized-broadcast-async.xml"
    shadow_plugin = "PEER_POWCONSENSUS"

    os.system("rm -rf %s" % datadir)

    for i, arg in enumerate(sys.argv):
        if "--nodenum" == arg:
            nodenum = int(sys.argv[i+1])
            setup_multiple_node_xml_centralized(nodenum)
            shadow_configfile = "centralized-broadcast-async-large.xml"
            
    ## run shadow for test
    # returnCode = os.system("~/.shadow/bin/shadow -d %s %s" % (datadir, shadow_configfile))

    shadow = Popen([os.path.expanduser("~/.shadow/bin/shadow"), "-d", datadir, shadow_configfile], stdout=PIPE)

    shadow_stdout_filename = "shadow.output"
    shadow_stdout_file = open(shadow_stdout_filename, 'w')
    while shadow.poll() is None:
        l = shadow.stdout.readline()
        print l.strip()
        shadow_stdout_file.write(l)
    shadow_stdout_file.close()
    
    os.system("mv %s ./%s/%s" % (shadow_stdout_filename, datadir, shadow_stdout_filename))
    shadow_stdout_file = open("./%s/%s" % (datadir, shadow_stdout_filename),'r')

    shadow_returnCode = shadow.returncode
    for line in shadow_stdout_file:
        if "critical" in line:
            print "critical error is occurred during shadow simulation"
            sys.exit(-1)

    if shadow_returnCode != 0:
        print "test failed"
        sys.exit(-1)
    

    ## check Block idx and Block nonce (consensus over p2p network)
    outputfile1 = "./%s/hosts/bleep1/stdout-bleep1.%s.1000.log" % (datadir, shadow_plugin)
    outputfile2 = "./%s/hosts/bleep2/stdout-bleep2.%s.1000.log" % (datadir, shadow_plugin)
    outputfile3 = "./%s/hosts/bleep3/stdout-bleep3.%s.1000.log" % (datadir, shadow_plugin)
    outputfile9 = "./%s/hosts/bleep9/stdout-bleep9.%s.1000.log" % (datadir, shadow_plugin)

    test_pass = True
    bleep1_blockidx_blocknonce = {}
    if os.path.exists(outputfile1):
        f = open(outputfile1)
        for line in f:
            if "Block idx=" in line:
                block_idx = int(line.split(":")[1].split(",")[0].split("=")[1])
                block_nonce = line.split(":")[1].split(",")[1].split("=")[1]
                bleep1_blockidx_blocknonce[block_idx] = block_nonce

    if len(bleep1_blockidx_blocknonce) == 0:
        print "test failed. No valid block is propagated"
        sys.exit(-1)

    bleep2_blockidx_blocknonce = {}
    if os.path.exists(outputfile2):
        f = open(outputfile2)
        for line in f:
            if "Block idx=" in line:
                block_idx = int(line.split(":")[1].split(",")[0].split("=")[1])
                bleep2_block_nonce = line.split(":")[1].split(",")[1].split("=")[1]
                bleep2_blockidx_blocknonce[block_idx] = bleep2_block_nonce
        for idx in sorted(bleep1_blockidx_blocknonce.keys()):
            if bleep1_blockidx_blocknonce[idx] != bleep2_blockidx_blocknonce[idx]:
                print "test failed. consensed block is different for bleep1, bleep2"
                print "block_idx=%d, bleep1_block_nonce=%s, bleep2_block_nonce=%s" % (idx, bleep1_blockidx_blocknonce[idx], bleep2_blockidx_blocknonce[idx])
                sys.exit(-1)

    bleep3_blockidx_blocknonce = {}
    if os.path.exists(outputfile3):
        f = open(outputfile3)
        for line in f:
            if "Block idx=" in line:
                block_idx = int(line.split(":")[1].split(",")[0].split("=")[1])
                bleep3_block_nonce = line.split(":")[1].split(",")[1].split("=")[1]
                bleep3_blockidx_blocknonce[block_idx] = bleep3_block_nonce
        for idx in sorted(bleep1_blockidx_blocknonce.keys()):
            if bleep1_blockidx_blocknonce[idx] != bleep3_blockidx_blocknonce[idx]:
                print "test failed. consensed block is different for bleep1, bleep3"
                print "block_idx=%d, bleep1_block_nonce=%s, bleep3_block_nonce=%s" % (idx, bleep1_blockidx_blocknonce[idx], bleep3_blockidx_blocknonce[idx])
                sys.exit(-1)
    
    bleep9_blockidx_blocknonce = {}
    if os.path.exists(outputfile9):
        f = open(outputfile9)
        for line in f:
            if "Block idx=" in line:
                block_idx = int(line.split(":")[1].split(",")[0].split("=")[1])
                bleep9_block_nonce = line.split(":")[1].split(",")[1].split("=")[1]
                bleep9_blockidx_blocknonce[block_idx] = bleep9_block_nonce
        for idx in sorted(bleep1_blockidx_blocknonce.keys()):
            if bleep1_blockidx_blocknonce[idx] != bleep9_blockidx_blocknonce[idx]:
                print "test failed. consensed block is different for bleep1, bleep9"
                print "block_idx=%d, bleep1_block_nonce=%s, bleep9_block_nonce=%s" % (idx, bleep1_blockidx_blocknonce[idx], bleep9_blockidx_blocknonce[idx])
                sys.exit(-1)
