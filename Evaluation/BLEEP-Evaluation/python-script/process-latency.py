import argparse
import os
import operator
import sys
import fileinput
    
if __name__ == '__main__':
    time_record = open("tmp-network-start-time.txt",'r')
    time_start  = int(time_record.readline().split('\0')[0])
    time_record.close()
    
    msg_id_dict = {}
    first_msg_time = -1
    first_msg_id   = -1
    msg_injection_time = {}

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
        injected_time = (received_time-time_start)/10000*10000
        time_interval = (received_time-time_start)-injected_time

        mid = int(parsing[3])
        arr = msg_id_dict.get(mid)
        if arr == None:
            msg_id_dict[mid] = []
        msg_id_dict[mid].append(time_interval)

        if first_msg_time == -1 or first_msg_time > injected_time:
            first_msg_time = injected_time
            first_msg_id   = mid
        msg_injection_time[mid] = injected_time

    latency_avg_set = []
    print "\n------------------------------------------------------------------------------------------------------------------------"
    print "Message ID ".rjust(20), "|", "injection time(sec)".rjust(20), "|", "# of nodes received".rjust(20), "|","Avg Latency(ms)".rjust(15), "|", "Min Latency(ms)".rjust(15), "|", "Max Latency(ms)".rjust(15)
    print "------------------------------------------------------------------------------------------------------------------------"
    for mid in msg_id_dict:
        if mid == first_msg_id:
            continue
        arr = msg_id_dict[mid]
        latency_avg = sum(arr, 0.0)/len(arr)
        message_cnt = len(arr)
        print str(mid).ljust(20), "|", str(float(msg_injection_time[mid])/1000.0).rjust(20), "|", str(message_cnt).rjust(20), "|", str(latency_avg).rjust(15), "|", str(float(min(arr))).rjust(15), "|", str(float(max(arr))).rjust(15)
        latency_avg_set.append(latency_avg)
    print "------------------------------------------------------------------------------------------------------------------------"
    
    total_latency_avg = sum(latency_avg_set, 0.0)/len(latency_avg_set)
    print "latency (avg):", str(total_latency_avg).rjust(10), "msec"