import os
from subprocess import check_output
import argparse
import sys

import networkx as nx
import matplotlib.pyplot as plt
edge = []
color = []
overlay = []
overlay_color = []

def check_overlay(i, j):
    for idx in range(len(overlay)):
        a1 = overlay[idx][0]
        a2 = overlay[idx][1]
        if i==a1 and j==a2:
            return 0

    overlay.append((i,j))
    return 1

def set_edge_color(nodes):
    for idx in range(len(nodes)):
        overlay_color.append('lightgray')
        
    for idx in range(len(overlay)):
        i = nodes.index(overlay[idx])
        if overlay_color[i] == 'lightgray':
            overlay_color[i] = 'darkgreen'            
        
def check_edge(i, j):
    for idx in range(len(edge)):
        a1 = edge[idx][0]
        a2 = edge[idx][1]
        if (i==a1 and j==a2) or (i==a2 and j==a1):
            del edge[idx]
            return 0

    edge.append([i,j])
    return 1

def set_node_color(num, nodes):
    #if use injector, set num + (# of injector)
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
    if len (sys.argv) != 2 :
        print "Command line input error"
        sys.exit (1)

    num = int(sys.argv[1])
    datadir = "shadow.data"
    shadow_plugin = "plugin"

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

            nodes = lineList[-1].split('\n')[0].split(',')
            if nodes[0] == "":
                continue
            
            for dst in nodes:
                node  = "%d" % (i)
                state = dst[-2]
                if int(state) == 1:
                    check_overlay(node,dst[5:-3])
               
                Graph.add_edge(node, dst[5:-3])
                check_edge(node,dst[5:-3])

                #if int(state) == 1: 
                #    Graph.add_edge(node, dst[5:-3], color='r')
                #else: 
                #    Graph.add_edge(node, dst[5:-3], color='b')
                

    set_node_color(num, list(Graph.node()))
    set_edge_color(list(Graph.edges()))

    #nx.draw(Graph, node_color=color_map, with_labels = True)
    #pos = nx.circular_layout(Graph)
    #nx.draw_circular(Graph, with_labels = True)
    #nx.draw_random(Graph, with_labels = True)
    #nx.draw_spring(Graph, with_labels = True) 
    nx.draw(Graph, with_labels = True, node_color = color, edge_color = overlay_color, width=2)
    plt.show()
