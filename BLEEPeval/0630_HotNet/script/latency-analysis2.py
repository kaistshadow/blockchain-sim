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

    lat_list = []
    lat_dict = {}

    for root, dirs, files in os.walk(directory):
        for logfile in files:
            print logfile
            if len(logfile.split("-")) == 3:
                nodename = logfile.split("-")[2]
            else:
                nodename = logfile.split("-")[1].split(".")[0]
            filepath = os.path.join(root,logfile)


            begin = 0
            beginline = ""
            beginlinenum = 0
            for i, line in enumerate(open(filepath, 'r')):
                if "before ev_run" in line:
                    begin = float(line.split(":")[2])
                    beginline = line
                    beginlinenum = i
                
                if begin != 0 and "dataSocketIOCallback called" in line and i-1 == beginlinenum:
                    end = float(line.split(":")[2])
                    lat = end - begin
                    # lat_list.append(lat)
                    begin = 0
                    beginlinenum = 0
                    # print beginline.strip()
                    # print line.strip()
                    # print '{:f}'.format(lat)
                    lat_dict[lat] = (beginline.strip(), line.strip())


    for lat in sorted(lat_dict.items()):
        print lat
    exit(0)

        

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for latency analysis')
    parser.add_argument("directory", help="directory path for result log files")

    args = parser.parse_args()
    directory = args.directory

    analysis_logs(directory)

