from collections import defaultdict
import argparse
import os


def analysis_logs(directory):
    rtt = defaultdict(list) # (src,dest) : [latency,..]
    for root, dirs, files in os.walk(directory):
        for logfile in files:
            senttime = {} # dest : senttime
            if len(logfile.split("-")) == 3:
                nodename = logfile.split("-")[2]
            else:
                nodename = logfile.split("-")[1].split(".")[0]
            filepath = os.path.join(root,logfile)            
            for line in open(filepath, 'r'):
                if "RTTReq" in line:
                    print line
                    dest = line.split()[4]
                    time = line.split(":")[2]
                    senttime[dest] = time
                elif "RTTRep" in line:
                    dest = line.split()[4]
                    time = line.split(":")[2]
                    rtt[(nodename, dest)].append(float(time) - float(senttime[dest]))

    rtt = sorted(rtt.items(), key=lambda x: x[0][1])
    rtt = sorted(rtt, key=lambda x: x[0][0])
    for ((src,dest), lats) in rtt:
        print "%s to %s rtt. avg=%f, max=%f, min=%f" % (src, dest, sum(lats)/len(lats), max(lats), min(lats))


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for rtt latency analysis')
    parser.add_argument("directory", help="directory path for result log files")
    
    args = parser.parse_args()
    directory = args.directory

    analysis_logs(directory)
