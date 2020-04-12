from subprocess import check_output
import os
import argparse
import sys
import networkx as nx
import matplotlib.pyplot as plt
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

# -----------------------------------------
if __name__ == '__main__':
    if len (sys.argv) < 2 or len (sys.argv) > 4:
        print "Command line input error"
        print "network_graph_proxy.py <num>"
        sys.exit (1)
    else:
        print "\n--- [Proxy Network Topology] ---"

    # Set basic parameters
    shadow_plugin = "PEER"
    datadir = "rc1-datadir"
    num  = int(sys.argv[1])

    # [Basic setup for analysis: IP_MAP, time_start]
    # Find IP allocated by using SetHostIp log data
    is_exit = False
    for i in range(0, num):
        outputfile = "./%s/hosts/bleep%d/stdout-bleep%d.%s.1000.log" % (datadir, i, i, shadow_plugin)
        if os.path.exists(outputfile):
            fileHandle = open(outputfile,"r" )
            lineList = fileHandle.readlines()
            fileHandle.close()

            idx = 0
            ip  = ""
            is_exit = False
            while True:
                parsing = lineList[idx].split('\n')[0].split(' ')
                first   = parsing[0]
                if first == "SetHostIP":
                    ip = parsing[2]
                    if ip_map.get(ip) == None:
                        ip_map[ip] = i
                    else:
                        print "[IP_MAP] %s already has different id not %d" % (ip,i)
                        print ip_map.get(ip)
                        is_exit = True
                    break

                idx = idx + 1
                if idx == len(lineList):
                    print "[IP_MAP] no IP exists for %d" % i
                    is_exit = True
                    break
            if is_exit:
                sys.exit(1)

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
                    break

                idx = idx -1
                if idx + len(lineList) == -1:
                    print "[NetworkGraph] no log exists"
                    is_exit = True
                    break
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
                Graph.add_edge(node, node_id)
                check_edge(node, node_id)

    # [Draw network graph]
    set_node_color(num, list(Graph.node()))
    set_edge_color(list(Graph.edges()))
    nx.draw(Graph, with_labels = True, node_color = color, edge_color = overlay_color, width=2)
    plt.show()
