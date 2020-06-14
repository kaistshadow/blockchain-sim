import numpy as np
import pandas as pd

import seaborn as sns
import matplotlib.pyplot as plt

import argparse

def multispace_remover(d):
    res = ""
    space_flag = 0
    for i in range(len(d)):
        if d[i] == " ":
            if space_flag == 0:
                res += ','
            space_flag = 1
        else:
            res += d[i]
            space_flag = 0
    return res
def korean_time_to_general(time_str):
    res = ""
    res += time_str[0:2]+":"
    res += time_str[6:8]+":"
    res += time_str[12:14]
    return res

def make_db_partition(partition_str, corecnt, workercnt, time_idx):
    res = ""
    lines = partition_str.split('\n')
    if partition_str[0:7] == "Average":
        # average setting
        partition_time = "Average"
        for line in lines:
            res += line[8:]+"\n"
    elif partition_str[2] == ':':
        partition_time = lines[1][0:7]
        for line in lines:
            res += line[11:]+"\n"
    else:
        partition_time = korean_time_to_general(lines[1][0:15])
        for line in lines:
            res += line[15:]+"\n"
    res = multispace_remover(res)
    
    lines = res.split('\n')
    arr = list()
    for line in lines:
        if line == "":
            continue
        row = line.split(',')[1:]
        arr.append(row)
    
    wsize = 1
    if workercnt < 2:
        wsize = 1
    elif workercnt > corecnt:
        wsize = corecnt
    else:
        wsize = workercnt
    # starting core no (from 0)
    wstart = corecnt - wsize
    
    usrList = list()
    sysList = list()
    totalList = list()
    for i in range(corecnt-wstart):
        usrElement = float(arr[wstart+i+2][1])
        sysElement = float(arr[wstart+i+2][3])
        usrList.append(usrElement)
        sysList.append(sysElement)
        totalList.append(usrElement+sysElement)
    
    # for counting
    granularity = 5
    usrRangeList = [0] * int(100 / granularity)
    sysRangeList = [0] * int(100 / granularity)
    totalRangeList = [0] * int(100 / granularity)
    RangeIndex = [0] * int(100 / granularity)
    for i in range(int(100 / granularity)):
        RangeIndex[i]=(i+1)*granularity
    for i in range(corecnt-wstart):
        curTarget = int(float(usrList[i])-0.0001)
        if curTarget < 0:
            curTarget = 0
        usrRangeList[curTarget//granularity] += 1
        curTarget = int(float(sysList[i])-0.0001)
        if curTarget < 0:
            curTarget = 0
        sysRangeList[curTarget//granularity] += 1
        curTarget = int(float(totalList[i])-0.0001)
        if curTarget < 0:
            curTarget = 0
        totalRangeList[curTarget//granularity] += 1
    #usrDf = pd.DataFrame(data=usrList, columns=[str(time_idx)])
    #sysDf = pd.DataFrame(data=sysList, columns=[str(time_idx)])
    #totalDf = pd.DataFrame(data=totalList, columns=[str(time_idx)])
    
    usrDf = pd.DataFrame(data=usrRangeList, index=RangeIndex, columns=[str(time_idx)]).sort_index(ascending=False)
    sysDf = pd.DataFrame(data=sysRangeList, index=RangeIndex, columns=[str(time_idx)]).sort_index(ascending=False)
    totalDf = pd.DataFrame(data=totalRangeList, index=RangeIndex, columns=[str(time_idx)]).sort_index(ascending=False)
    
    return usrDf, sysDf, totalDf

def log_to_db(filename, corecnt, workercnt):
    db = 1 # required to be set empty
    
    f = open(filename, "r")
    
    lines = f.readlines()
    line_idx = 1
    line_cnt = len(lines)
    
    timestamp_idx = 0
    startFlag = 1
    while(line_idx<line_cnt):
        
        line = lines[line_idx]
        line_idx+=1
        if line=="\n":
            continue
        else:
            db_partition_str = ""
            db_partition_str += line
            for i in range(corecnt+1):
                db_partition_str += lines[line_idx]
                line_idx+=1
            usr, sys, total = make_db_partition(db_partition_str, corecnt, workercnt, timestamp_idx)
            timestamp_idx += 1
            if startFlag == 1:
                usrDf = usr
                sysDf = sys
                totalDf = total
                startFlag = 0
            else:
                usrDf = pd.merge(usrDf, usr,left_index=True, right_index=True,how='outer')
                sysDf = pd.merge(sysDf, sys,left_index=True, right_index=True,how='outer')
                totalDf = pd.merge(totalDf, total,left_index=True, right_index=True,how='outer')
    usrDf = usrDf
    
    wsize = 1
    if workercnt < 2:
        wsize = 1
    elif workercnt > corecnt:
        wsize = corecnt
    else:
        wsize = workercnt

    ax = sns.heatmap(usrDf, annot=True, fmt="g", vmax=wsize, vmin=0)
    fig = ax.get_figure()
    fig.savefig(filename+"usr.png")
    fig.clf()
    ax = sns.heatmap(sysDf, annot=True, fmt="g", vmax=wsize, vmin=0)
    fig = ax.get_figure()
    fig.savefig(filename+"sys.png")
    fig.clf()
    ax = sns.heatmap(totalDf, annot=True, fmt="g", vmax=wsize, vmin=0)
    fig = ax.get_figure()
    fig.savefig(filename+"total.png")
    fig.clf()
    f.close()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script to make figure from mpstat logs')
    parser.add_argument("target", help="target mpstat log")
    parser.add_argument("corecnt", help="machine core count")
    parser.add_argument("workercnt", help="requested Shadow worker thread count")

    args = parser.parse_args()
    target = args.target
    corecnt = int(args.corecnt)
    workercnt = int(args.workercnt)

    log_to_db("./mpstat_results/"+target, corecnt, workercnt)