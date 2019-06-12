from collections import defaultdict
import argparse
import os

def cmp_step(item1, item2):
    if item1[0] == "BlockMined":
        return -1
    elif item2[0] == "BlockMined":
        return 1
    else:
        return 0

def analysis_logs(directory):
    blk = defaultdict(list) # msghash : [ (Step, Time, at) ]
    for root, dirs, files in os.walk(directory):
        for logfile in files:
            if len(logfile.split("-")) == 3:
                nodename = logfile.split("-")[2]
            else:
                nodename = logfile.split("-")[1].split(".")[0]
            filepath = os.path.join(root,logfile)
            for line in open(filepath, 'r'):
                if "PrintBlockTimeLogs" in line:
                    blk[line.split(",")[1]].append( (line.split(",")[2],line.split(",")[3].strip(), nodename))

    for msghash, tuples in blk.items():
        sorted_tuples = sorted(tuples, key=lambda x: x[1]) # sort by time
        sorted_tuples = sorted(sorted_tuples, key=lambda x: x[2]) # sort by nodename
        
        earliest_time = {}
        for (step, time, at) in sorted_tuples:
            if not at in earliest_time:
                earliest_time[at] = time

        # get sorted nodenames, earliest node first
        sorted_nodenames = [x[0] for x in sorted(earliest_time.items(), key=lambda x: x[1])]

        # sort for following sorted nodenames
        sorted_tuples = sorted(sorted_tuples, key=lambda x : sorted_nodenames.index(x[2]))

        # sorted_tuples = sorted(sorted_tuples, cmp=cmp_step) # show blockmined event first 

        print "==="
        for (step, time, at) in sorted_tuples:
            print "%s %s %s" % (step, time, at)

        

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for latency analysis')
    parser.add_argument("directory", help="directory path for result log files")

    args = parser.parse_args()
    directory = args.directory

    analysis_logs(directory)

