import os
from subprocess import check_output, Popen, PIPE
import argparse
import sys

if __name__ == '__main__':
    datadir = "centralized-broadcast-datadir"
    shadow_configfile = "centralized-broadcast.xml"
    shadow_plugin = "PEER_POWCONSENSUS"

    ## run shadow for test
    # returnCode = os.system("~/.shadow/bin/shadow -d %s %s" % (datadir, shadow_configfile))

    shadow = Popen([os.path.expanduser("~/.shadow/bin/shadow"), "-d", datadir, shadow_configfile], stdout=PIPE)
    shadow_stdout = shadow.communicate()[0]
    shadow_returnCode = shadow.returncode
    for line in shadow_stdout:
        if "critical" in shadow_stdout:
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
