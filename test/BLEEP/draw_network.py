import os
from subprocess import check_output
import argparse
import sys

import networkx as nx
import matplotlib.pyplot as plt

# Arrays to build HyparView overlay
edge = []
color = [] # for node color
# Arrays to build Plumtree overlay
overlay = []
overlay_color = [] # for edge color

# Measurement
start_time = 0
end_time   = 0

# Check Edges and their state to make PlumTree Overlay
def check_overlay(i, j):
    for idx in range(len(overlay)):
        a1 = overlay[idx][0]
        a2 = overlay[idx][1]
        if i==a1 and j==a2:
            return 0
    overlay.append((i,j))
    return 1

# Set color of PlumTree overlay's edges, Eager edges
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

# Check Edges to make HyparView Overlay
def check_edge(i, j):
    for idx in range(len(edge)):
        a1 = edge[idx][0]
        a2 = edge[idx][1]
        if (i==a1 and j==a2) or (i==a2 and j==a1):
            del edge[idx]
            return 0
    edge.append([i,j])
    return 1

# Set color of HyParView overlay's nodes, red for problematic nodes
def set_node_color(num, nodes):
    #if use injector, set num + (# of injector) if want to display injector
    for idx in range(num):
        color.append('white')
        
    for idx in range(len(edge)):
        a1 = nodes.index(edge[idx][0])
        a2 = nodes.index(edge[idx][1])
        if color[a1] == 'white':
            color[a1] = 'red'            
        if color[a2] == 'white':
            color[a2] = 'red'


if __name__ == '__main__':
    if len (sys.argv) < 2 :
        print "Command line input error"
        print "draw_network.py <num>"
        print "draw_network.py <num><datadir>"
        print "draw_network.py <num><datadir><time>"
        sys.exit (1)

    # Set basic parameters
    num = int(sys.argv[1])
    shadow_plugin = "plugin"
    datadir = "plumtree-broadcast-datadir"
    if len (sys.argv) >= 3:
        shadow_plugin = "PEER_POWCONSENSUS"
        datadir = sys.argv[2]
        print datadir

    if len (sys.argv) == 4:
        stop_time = int(sys.argv[3])
    else:
        stop_time = -1

    # Add Nodes
    Graph = nx.DiGraph()
    for i in range(1,num+1):
        node = "%d" % (i)
        Graph.add_node(node)
    
    # Process Time related functions and Add Edges
    for i in range(1,num+1):
        outputfile = "./%s/hosts/bleep%d/stdout-bleep%d.%s.1000.log" % (datadir, i, i, shadow_plugin)
        if os.path.exists(outputfile):
            fileHandle = open(outputfile,"r" )
            lineList = fileHandle.readlines()
            fileHandle.close()

            # find start time of first node
            if i == 1:
                start_time = int(lineList[2].split('\n')[0][11:])

            # To find the latest gossip overlay info about edges and their states
            idx  = -1
            flag = 0
            while True:
                while lineList[idx] == '\n' or lineList[idx][0:5] != 'bleep':
                    idx = idx -1
                    if len(lineList)+idx < 0:
                        flag = 1
                        break               
                if flag:
                    break
                if stop_time != -1:
                    time = int(lineList[idx].split(' ')[1].split('\n')[0][5:])
                    if time <= start_time+stop_time: 
                        break
                    else:
                        idx = idx -1
                else:
                    break
                
            if flag:
                continue

            # reflect edges state to gossip overlay
            nodes = lineList[idx].split(' ')[0].split(',')
            if nodes[0] == "":
                continue
            time = int(lineList[idx].split(' ')[1].split('\n')[0][5:])
            end_time = max(end_time,time)

            for dst in nodes:
                node  = "%d" % (i)
                state = dst[-2]    # state 1 : Eager and 0 : Lazy 
                if int(state) == 1:
                    check_overlay(node,dst[5:-3])
                Graph.add_edge(node, dst[5:-3])
                check_edge(node,dst[5:-3])

    if len(sys.argv) == 4:
        print "start time = %d, stop time = %d, elapsed time = %d" % (start_time, stop_time+start_time, stop_time)
    else:
        print "start time = %d, end time = %d, elapsed time = %d" % (start_time, end_time, end_time-start_time)

    set_node_color(num, list(Graph.node()))
    set_edge_color(list(Graph.edges()))

    nx.draw(Graph, with_labels = True, node_color = color, edge_color = overlay_color, width=2)
    #nx.draw(Graph, with_labels = True, node_color = color, width=2)\

    plt.show()
