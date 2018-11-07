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
        sys.exit (1)

    num = int(sys.argv[1])
    shadow_plugin = "plugin"
    datadir = "plumtree-broadcast-datadir"
    if len (sys.argv) == 3:
        shadow_plugin = "PEER_POWCONSENSUS"
        datadir = sys.argv[2]
        print datadir

    Graph = nx.DiGraph()
    for i in range(1,num+1):
        node = "%d" % (i)
        Graph.add_node(node)

    for i in range(1,num+1):
        outputfile = "./%s/hosts/bleep%d/stdout-bleep%d.%s.1000.log" % (datadir, i, i, shadow_plugin)
        if os.path.exists(outputfile):
            fileHandle = open(outputfile,"r" )
            lineList = fileHandle.readlines()
            fileHandle.close()

            # TO find the latest gossip overlay info about edges and their states
            idx = -1
            while lineList[idx] == '\n' or lineList[idx][0:5] != 'bleep':
                idx = idx -1
            #print i, idx, lineList[idx]

            nodes = lineList[idx].split('\n')[0].split(',')
            if nodes[0] == "":
                continue
            
            for dst in nodes:
                node  = "%d" % (i)
                # state 1 : Eager and 0 : Lazy 
                state = dst[-2] 
                if int(state) == 1:
                    check_overlay(node,dst[5:-3])
               
                Graph.add_edge(node, dst[5:-3])
                check_edge(node,dst[5:-3])

    set_node_color(num, list(Graph.node()))
    set_edge_color(list(Graph.edges()))

    nx.draw(Graph, with_labels = True, node_color = color, edge_color = overlay_color, width=2)
    #nx.draw(Graph, with_labels = True, node_color = color, width=2)

    plt.show()
