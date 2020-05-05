import argparse
import os
import operator
import sys

if __name__ == '__main__':
    # input = # of nodes except injector for the last evalution
    if len (sys.argv) != 2:
        print "Command line input error"
        sys.exit (1)
    num = int(sys.argv[1])

    # Set basic parameters
    shadow_plugin = "PEER"
    datadir = "rc1-datadir"
    
    # Crawling the tx received data
    f = open("gossip-injected-msg.txt",'w')
    p = open("gossip-protocol-msg.txt",'w')
    
    for i in range(0, num):
        outputfile = "./%s/hosts/bleep%d/stdout-bleep%d.%s.1000.log" % (datadir, i, i, shadow_plugin)
        if os.path.exists(outputfile):
            fileHandle = open(outputfile,"r" )
            lineList   = fileHandle.readlines()
            fileHandle.close()

            for idx in range(0, len(lineList)):
                parsing = lineList[idx].split('\n')[0].split(' ')
                first   = parsing[0]
                if first == "NEWMSG":
                    f.write(lineList[idx])
                elif first == "PLUMTREE":
                    p.write(lineList[idx])
                else:
                    continue

    f.close()
    p.close()
