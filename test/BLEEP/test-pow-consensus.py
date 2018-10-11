import os
from subprocess import check_output
import argparse
import sys

if __name__ == '__main__':
    datadir = "pow-consensus-datadir"
    shadow_configfile = "pow-consensus.xml"
    shadow_plugin = "PEER_POWCONSENSUS"
    # shadow_inject_plugin = "INJECTOR_STELLARCONSENSUS"

    os.system("rm -rf %s" % datadir)

    ## run shadow for test
    returnCode = os.system("~/.shadow/bin/shadow -d %s %s" % (datadir, shadow_configfile))
    if returnCode != 0:
        print "test failed"
        sys.exit(-1)

    ## check Block idx and Block nonce (consensus over p2p network)
    outputfile1 = "./%s/hosts/bleep1/stdout-bleep1.%s.1000.log" % (datadir, shadow_plugin)
    outputfile2 = "./%s/hosts/bleep2/stdout-bleep2.%s.1000.log" % (datadir, shadow_plugin)
    outputfile3 = "./%s/hosts/bleep3/stdout-bleep3.%s.1000.log" % (datadir, shadow_plugin)
    test_pass = True
    bleep1_blockidx_blocknonce = {}
    if os.path.exists(outputfile1):
        f = open(outputfile1)
        for line in f:
            if "Block idx=" in line:
                block_idx = line.split(":")[1].split(",")[0].split("=")[1]
                block_nonce = line.split(":")[1].split(",")[1].split("=")[1]
                bleep1_blockidx_blocknonce[block_idx] = block_nonce
    
    if os.path.exists(outputfile2):
        f = open(outputfile2)
        for line in f:
            if "Block idx=" in line:
                block_idx = line.split(":")[1].split(",")[0].split("=")[1]
                bleep2_block_nonce = line.split(":")[1].split(",")[1].split("=")[1]
                if bleep1_blockidx_blocknonce[block_idx] != bleep2_block_nonce:
                    print "test failed. consensed block is different for bleep1, bleep2"
                    print "block_idx=%s, bleep1_block_nonce=%s, bleep2_block_nonce=%s" % (block_idx, bleep1_blockidx_blocknonce[block_idx], bleep2_block_nonce)
                    sys.exit(-1)
                    
    if os.path.exists(outputfile3):
        f = open(outputfile3)
        for line in f:
            if "Block idx=" in line:
                block_idx = line.split(":")[1].split(",")[0].split("=")[1]
                bleep3_block_nonce = line.split(":")[1].split(",")[1].split("=")[1]
                if bleep1_blockidx_blocknonce[block_idx] != bleep3_block_nonce:
                    print "test failed. consensed block is different for bleep1, bleep3"
                    print "block_idx=%s, bleep1_block_nonce=%s, bleep3_block_nonce=%s" % (block_idx, bleep1_blockidx_blocknonce[block_idx], bleep3_block_nonce)
                    sys.exit(-1)
    

