from subprocess import check_output
import os
import argparse
import sys
import networkx as nx
import matplotlib.pyplot as plt
import invariant_network_gossiptree as cic
import operator
import math

# -----------------------------------------
# ip-domain map
ip_map = {}
# [MEMBERSHIP] Arrays to build HyparView overlay
edge  = []
color = []
# [GOSSIP] Arrays to build Plumtree overlay
overlay = []
overlay_color = []
# [MEMBERSHIP][GOSSIP] Measurement related parameters
start_time = 0
end_time   = 0
# [MEMBERSHIP][GOSSIP] CHECK_INVARIANT_CYCLE
# check existance of circle using undirected version graph edge
undirected_edge = []
undirected_edge_temp = []

# -----------------------------------------
# [MEMBERSHIP] Check Edges to make HyparView Overlay
def check_edge(i, j):
    for idx in range(len(edge)):
        a1 = edge[idx][0]
        a2 = edge[idx][1]
        if (i==a1 and j==a2) or (i==a2 and j==a1):
            del edge[idx]
            return 0
    edge.append([i,j])
    return 1

# [MEMBERSHIP] Set color of HyParView overlay's nodes, red for problematic nodes
def set_node_color(num, nodes):
    for idx in range(num):
        color.append('white')

    for idx in range(len(edge)):
        a1 = nodes.index(edge[idx][0])
        a2 = nodes.index(edge[idx][1])
        if color[a1] == 'white':
            color[a1] = 'red'
        if color[a2] == 'white':
            color[a2] = 'red'

# [GOSSIP] Check Edges and their state to make PlumTree Overlay
def check_overlay(i, j):
    for idx in range(len(overlay)):
        a1 = overlay[idx][0]
        a2 = overlay[idx][1]
        if i==a1 and j==a2:
            return 0
    overlay.append((i,j))
    return 1

# [GOSSIP] Set color of PlumTree overlay's edges, Eager edges
def set_edge_color(edges):
    for idx in range(len(edges)):
        overlay_color.append('lightgray')

    for idx in range(len(overlay)):
        i = edges.index(overlay[idx])
        a1 = overlay[idx][0]
        a2 = overlay[idx][1]
        if (a2,a1) in edges:
            j = edges.index((a2,a1))
            if overlay_color[i] == 'lightgray':
                overlay_color[i] = 'red'
                overlay_color[j] = 'red'
            elif overlay_color[i] == 'red':
                overlay_color[i] = 'darkgreen'
                overlay_color[j] = 'darkgreen'
        else:
            overlay_color[i] = 'red'

# [MEMBERSHIP][GOSSIP] CHECK_INVARIANT_CYCLE
def add_undirected_graph_edge(i, j):
    for idx in range(len(undirected_edge_temp)):
        a1 = undirected_edge_temp[idx][0]
        a2 = undirected_edge_temp[idx][1]
        if (i==a1 and j==a2) or (i==a2 and j==a1):
            del undirected_edge_temp[idx]
            undirected_edge.append([i,j])
            return 1
    undirected_edge_temp.append([i,j])
    return 0

# -----------------------------------------
if __name__ == '__main__':
    if len (sys.argv) < 2 or len (sys.argv) > 4:
        print "Command line input error"
        print "./python-script/check_invariant.py <num>"
        print "./python-script/check_invariant.py <num><time>"
        sys.exit (1)
    else:
        print "\n--- [Check Invariants & Network Topology] ---"

    # Set basic parameters
    shadow_plugin = "PEER"
    datadir = "rc1-datadir"
    num  = int(sys.argv[1])

    # snapshot related parameters
    time = -1
    time_start = -1
    time_end   = -1
    time_interval = 0
    if len(sys.argv) == 3:
        time_interval = float(sys.argv[2])

    # [Basic setup for analysis: IP_MAP, time_start]
    # Find IP allocated by using SetHostIp log data
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

    # [Draw network graph]
    set_node_color(num, list(Graph.nodes()))
    set_edge_color(list(Graph.edges()))
    nx.draw(Graph, with_labels = True, node_color = color, edge_color = overlay_color, width=2)
    plt.show()
