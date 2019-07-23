from collections import defaultdict
import argparse
import os

sentPOWBlkNum = defaultdict(list) # nodename : [sentNum0, sentNum1, ...]
sentPOWMsgNum = defaultdict(list) # nodename : [sentNum0, sentNum1, ...]
appliedBlkNum = defaultdict(list) # nodename : [BlkNum0, BlkNum1, ...]

expCount = 0
sentPOWBlkNumTotal = 0
sentPOWMsgNumTotal = 0
recvPOWBlknumforkTotal = 0
minedBlknumTotal = 0
appliedBlknumTotal = 0
millisecondTotal = 0

def analysis_stats(directory, nodenum = 6):
    global expCount, sentPOWBlkNumTotal, sentPOWMsgNumTotal, recvPOWBlknumforkTotal, minedBlknumTotal, appliedBlknumTotal, millisecondTotal

    updateCount = 0
    nodeCount = 0
    milliseconds = []
    for root, dirs, files in os.walk(directory):
        for logfile in files:
            if not "stdout" in logfile:
                continue
            if len(logfile.split("-")) == 3:
                nodename = logfile.split("-")[2]
            else:
                nodename = logfile.split("-")[1].split(".")[0]
            filepath = os.path.join(root,logfile)            
            for line in open(filepath, 'r'):
                if "sentPOWBlknum" in line:
                    value = int(line.split("=")[1])
                    sentPOWBlkNum[nodename].append(value)
                    sentPOWBlkNumTotal += value
                    updateCount += 1
                elif "sentPOWMsgnum" in line:
                    value = int(line.split("=")[1])
                    sentPOWMsgNum[nodename].append(value)
                    sentPOWMsgNumTotal += value
                    updateCount += 1
                elif "recvPOWBlknumfork" in line:
                    value = int(line.split("=")[1])
                    recvPOWBlknumforkTotal += value
                    updateCount += 1
                elif "total_mined_block_num" in line:
                    value = int(line.split("=")[1])
                    minedBlknumTotal += (value-1)
                    updateCount += 1
                elif "applied_mined_block_num" in line:
                    value = int(line.split("=")[1])
                    appliedBlknumTotal += (value -1)
                    appliedBlkNum[nodename].append(value-1)
                    updateCount += 1                    
                elif "milliseconds=" in line:
                    value = float(line.split("=")[1].strip())
                    milliseconds.append(value)
                    updateCount += 1                    
            if updateCount == 6:
                nodeCount += 1
                updateCount = 0
            elif updateCount == 0:
                pass
            else:
                print "illegal update!", logfile, updateCount


    if nodeCount == nodenum:
        millisecondTotal += max(milliseconds)
        expCount += 1
    elif nodeCount == 0:
        return
    else :
        print "illegal update!"
        print "nodeCount:",nodeCount
        exit(-1)
                    
                    

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for stat analysis')
    parser.add_argument("directory", help="directory path for result log files", nargs='+')
    parser.add_argument("--nodenum", help="number of nodes for exp")
    
    args = parser.parse_args()
    directories = args.directory

    nodenum = 6
    if args.nodenum:
        nodenum = int(args.nodenum)
    print nodenum

    with open("experiment_times.log","r") as f:
        count = 0
        durationTotal = 0
        for line in f:
            count += 1
            durationTotal += int(line.split("=")[1])
        print "average_exp_time=%f" % (float(durationTotal) / count)


    for directory in directories:
        analysis_stats(directory, nodenum)

    sentPOWBlkNum = sorted(sentPOWBlkNum.items(), key=lambda x: x[0])
    sentPOWMsgNum = sorted(sentPOWMsgNum.items(), key=lambda x: x[0])
    appliedBlkNum = sorted(appliedBlkNum.items(), key=lambda x: x[0])


    print "expCount:%d" % expCount

    for (nodename, nums) in sentPOWBlkNum:
        print "sentedPOWBlkNum for %s : %f" % (nodename, float(sum(nums))/len(nums))

    for (nodename, nums) in sentPOWMsgNum:
        print "sentedPOWMsgNum for %s : %f" % (nodename, float(sum(nums))/len(nums))

    for (nodename, nums) in appliedBlkNum:
        print "appliedBlkNum for %s : %f" % (nodename, float(sum(nums))/len(nums))

    print "sentPOWBlkNumTotal : %f" % (float(sentPOWBlkNumTotal)/expCount)
    print "sentPOWMsgNumTotal : %f" % (float(sentPOWMsgNumTotal)/expCount)
    print "sentrecvPOWBlkNumTotal : %f" % (float(sentPOWBlkNumTotal + recvPOWBlknumforkTotal)/ expCount)
    print "minedBlknumTotal : %f" % (float(minedBlknumTotal)/expCount)
    print "appliedBlknumTotal : %f" % (float(appliedBlknumTotal)/expCount)
    print "millisecond : %f" % (float(millisecondTotal)/(expCount))

