from subprocess import check_output
import os
import argparse
import sys
import networkx as nx
import operator
import math

if __name__ == '__main__':

    # Set basic parameters
    shadow_plugin = "PEER"
    datadir = "rc1-datadir"
    num = int(sys.argv[1])

    is_exit = False
    injectors = []
    for i in range(0, num):
        outputfile = "./%s/hosts/bleep%d/stdout-bleep%d.%s.1000.log" % (datadir, i, i, shadow_plugin)
        if os.path.exists(outputfile):
            fileHandle = open(outputfile,"r" )
            lineList = fileHandle.readlines()
            fileHandle.close()

            idx = 0
            ip  = ""
            is_exit = False
            cnt = 0
            while True:
                parsing = lineList[idx].split('\n')[0].split(' ')
                first   = parsing[0]
                if first == "SetHostIP":
                    ip = parsing[2]
                    if ip_map.get(ip) == None:
                        ip_map[ip] = i
                        cnt = cnt +1
                    else:
                        print "[IP_MAP] %s already has different id not %d" % (ip,i)
                        print ip_map.get(ip)
                        is_exit = True
                        break
                elif first == "StartTime":
                    log_time = int(parsing[1])
                    if time_start == -1:
                        time_start = log_time
                    else:
                        time_start = min(time_start, log_time)
                    cnt = cnt +1
                elif first == "[JoinNetwork]":
                    node_type = parsing[2]
                    if node_type == "INJECTOR":
                        injectors.append(int(i))
                    cnt = cnt +1

                if cnt == 3:
                    break
                idx = idx + 1
                if idx == len(lineList):
                    print "[IP_MAP] no IP exists for %d" % i
                    is_exit = True
                    break
            if is_exit:
                sys.exit(1)

    # Print the start point of graph analysis
    print "[Network] First node starts at", time_start
    if time_interval != 0:
        time = int(math.ceil(time_start + time_interval*1000))
        print "time :",time

    # [IP-DOMAIN MAP]
    print "[IP_MAP] :"
    cnt = 1
    ordered_ip_map = sorted(ip_map.items(), key=operator.itemgetter(1))
    for info in ordered_ip_map:
        print str(info[1]).ljust(3)+" : "+str(info[0]).ljust(10)+" |",
        if cnt == 4:
            print '\n',
            cnt = 1
        else:
            cnt = cnt +1
    print '\n'

    # [Add Nodes for Grap analysis]
    Graph = nx.DiGraph()
    for i in range(0, num):
        node = "%d" % (i)
        Graph.add_node(node)
    # [Process Edge for Graph analysis]
    for i in range(0, num):
        if i in injectors:
            continue

        outputfile = "./%s/hosts/bleep%d/stdout-bleep%d.%s.1000.log" % (datadir, i, i, shadow_plugin)
        if os.path.exists(outputfile):
            fileHandle = open(outputfile,"r" )
            lineList = fileHandle.readlines()
            fileHandle.close()

            # find lastest ActiveView log
            idx = -1
            is_exit = False
            while True:
                parsing = lineList[idx].split('\n')[0].split(' ')
                first   = parsing[0]
                if first == "[NetworkGraph]":
                    log_time = int(parsing[1])
                    if time == -1:
                        time_end = max(time_end, log_time)
                        break;
                    elif log_time <= time:
                        break;

                idx = idx -1
                if idx + len(lineList) == -1:
                    print "[NetworkGraph] no log exists"
                    is_exit = True
                    break;
            if is_exit:
                continue

            # reflect edges state to gossip overlay
            nodes = parsing[2].split(',')
            if nodes[0] == "":
                continue

            # x~x.x~x.x~x.x~x(x) format
            for dst in nodes:
                node  = "%d" % (i)
                state = dst[-2]
                ip    = dst[0:-3]
                node_id = str(ip_map.get(ip))
                if int(state) == 1:
                    check_overlay(node, node_id)
                    add_undirected_graph_edge(int(node), int(node_id)) # for CIC
                Graph.add_edge(node, node_id)
                check_edge(node, node_id)

    if time_end != -1:
        print "[Network] Stabilized after", time_end-time_start
    if len(injectors) != 0:
        print "[INJECTOR]", injectors
        for injector in injectors:
            Graph.add_edge(str(injector), "0")

    # [CHECK_INVARIANT_CYCLE test]
    print "[INVARIANT_NETWORK_GOSSIPTREE]",
    if len(undirected_edge_temp) == 0:
        start_point = undirected_edge[0][0]
        res = cic.cycle_detector(undirected_edge, start_point)
        if res[0]:
            print "Cycle exists"
        else:
            print "No cycle exists,",
            if res[1] == num-len(injectors):
                print "all node join the network"
            else:
                print "but some nodes are missing("+str(num-len(injectors)-res[1])+")"
    else:
        print "Non-symmetric network topology"
