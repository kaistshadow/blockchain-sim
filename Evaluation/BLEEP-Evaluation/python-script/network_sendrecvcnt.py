from subprocess import check_output
import os
import argparse
import sys

# -----------------------------------------
# count
recv_cnt = 0
send_cnt = 0

# -----------------------------------------
if __name__ == '__main__':
    if len (sys.argv) != 2:
        print "Command line input error"
        print "invariant_SRcount.py <num>"
        sys.exit (1)
    else:
        print "\n--- [Check Invariants Send-Recv Count] ---"

    # Set basic parameters
    shadow_plugin = "PEER"
    datadir = "rc1-gossip-datadir"
    num  = int(sys.argv[1])

    # Count totoal Send/Recv
    for i in range(0, num):
        outputfile = "./%s/hosts/bleep%d/stdout-bleep%d.%s.1000.log" % (datadir, i, i, shadow_plugin)
        if os.path.exists(outputfile):
            fileHandle = open(outputfile,"r" )
            lineList = fileHandle.readlines()
            fileHandle.close()

            idx = 0
            while True:
                parsing = lineList[idx].split('\n')[0].split(' ')
                first   = parsing[0]

                if first == "SEND":
                    send_cnt = send_cnt + 1
                elif first == "RECV":
                    recv_cnt = recv_cnt + 1

                idx = idx + 1
                if idx == len(lineList):
                    break

    print send_cnt, recv_cnt
