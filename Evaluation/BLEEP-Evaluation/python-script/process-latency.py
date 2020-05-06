import argparse
import os
import operator
import sys
import fileinput
    
if __name__ == '__main__':
    time_record = open("tmp-network-start-tim.txt",'r')
    time_start  = int(time_record.readline().split('\0')[0])
    time_record.close()
    
    msg_id_dict = {}
    tx_record   = "gossip-injected-msg.txt"
    for line in fileinput.input([tx_record]):
        parsing = line.split('\n')[0].split()
        if len(parsing) != 5:
            continue
        if parsing[0] != "NEWMSG":
            continue
        if parsing[1] != "NEW":
            continue
        
        received_time = int(parsing[2])
        time_interval = (received_time-time_start)-(received_time-time_start)/50000*50000

        mid = int(parsing[3])
        arr = msg_id_dict.get(mid)
        if arr == None:
            msg_id_dict[mid] = []
        msg_id_dict[mid].append(time_interval)

    latency_avg_set = []
    print "\n-------------------------------------------------------------------------------------"
    print "Message ID ".rjust(20), "|", "# of nodes received msg".rjust(25), "|","Avg Latency(ms)".rjust(15), "|", "Avg Latency(s)".rjust(15)
    print "-------------------------------------------------------------------------------------"
    for mid in msg_id_dict:
        arr = msg_id_dict[mid]
        latency_avg = sum(arr, 0.0)/len(arr)
        message_cnt = len(arr)
        print str(mid).ljust(20), "|", str(message_cnt).rjust(25), "|", str(latency_avg).rjust(15), "|", str(latency_avg/1000.0).rjust(15)
        latency_avg_set.append(latency_avg)
    print "-------------------------------------------------------------------------------------"
    
    total_latency_avg = sum(latency_avg_set, 0.0)/len(latency_avg_set)
    print "total avg latency:", str(total_latency_avg).rjust(15), "msec"
    print "total avg latency:", str(float(total_latency_avg)/1000.0).rjust(15), "sec"