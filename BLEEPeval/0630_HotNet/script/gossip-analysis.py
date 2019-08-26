from collections import defaultdict
import argparse
import os


def analysis_logs(filepath):
    msg = defaultdict(list) # (messageid) : [dest,..]
    msgset = defaultdict(set) # (messageid) : [(dest,time),..]

    for line in open(filepath, 'r'):
        if "EmuBlockMiningComplete" in line:
            minedInfo = line.split()[7]
            print minedInfo
            minedIndex = minedInfo.split(",")[6]
            time = minedInfo.split(",")[1]
            msg[minedInfo.split(",")[7]] = []
            msgset[minedInfo.split(",")[7]] = set()
            # if int(minedInfo.split(",")[6]) > 30:
            #     break
        elif "MulticastingMessage" in line:
            msgInfo = line.split()[7]
            fromNode = msgInfo.split(",")[3]
            toNode = msgInfo.split(",")[4]
            time = msgInfo.split(",")[1]
            msg[msgInfo.split(",")[6]].append(toNode)
            msgset[msgInfo.split(",")[6]].add(toNode)
            
    for hash, nodes in msg.items():
        print "For %s, total Multicasted num :%d, received node num:%d" % (hash, len(nodes), len(msgset[hash]))
        

    # rtt = sorted(rtt.items(), key=lambda x: x[0][1])
    # rtt = sorted(rtt, key=lambda x: x[0][0])
    # for ((src,dest), lats) in rtt:
    #     print "%s to %s rtt. avg=%f, max=%f, min=%f" % (src, dest, sum(lats)/len(lats), max(lats), min(lats))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for gossip analysis')
    parser.add_argument("filepath", help="file path for result log file")

    
    args = parser.parse_args()
    filepath = args.filepath

    analysis_logs(filepath)
