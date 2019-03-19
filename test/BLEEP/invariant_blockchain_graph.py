import sys
import re

import networkx as nx
import matplotlib.pyplot as plt

def hierarchy_pos(G, root, width=1., vert_gap = 0.2, vert_loc = 0, xcenter = 0.5,
                  pos = None, parent = None):
    '''If there is a cycle that is reachable from root, then this will see infinite recursion.
       G: the graph
       root: the root node of current branch
       width: horizontal space allocated for this branch - avoids overlap with other branches
       vert_gap: gap between levels of hierarchy
       vert_loc: vertical location of root
       xcenter: horizontal location of root
       pos: a dict saying where all nodes go if they have been assigned
       parent: parent of this branch.'''
    if pos == None:
        pos = {root:(xcenter,vert_loc)}
    else:
        pos[root] = (xcenter, vert_loc)
    neighbors = list(G.neighbors(root))
    # if parent != None:   #this should be removed for directed graphs.
    #     neighbors.remove(parent)  #if directed, then parent not in neighbors.
    if len(neighbors)!=0:
        dx = width/len(neighbors)
        nextx = xcenter - width/2 - dx/2
        for neighbor in neighbors:
            nextx += dx
            pos = hierarchy_pos(G,neighbor, width = dx, vert_gap = vert_gap,
                                vert_loc = vert_loc-vert_gap, xcenter=nextx, pos=pos,
                                parent = root)
    return pos

if __name__ == '__main__':
    if not(len (sys.argv) == 2 or len(sys.argv) == 3):
        print "Command line input error"
        print "USAGE: python draw_tree_test.py <nodenum>"
        print "USAGE: python draw_tree_test.py <nodenum> <datadir>"
        sys.exit (1)

    nodenum = int(sys.argv[1])
    datadir = "pow-consensus-datadir"
    if len(sys.argv) == 3:
        datadir = sys.argv[2]
    # shadow_plugin = "PEER_POWCONSENSUS"
    # datadir = "centralized-broadcast-datadir"
    # datadir = "pow-consensus-large-datadir"
    # shadow_plugin = "PEER_POWCONSENSUS"
    shadow_plugin = "PEER"

    labeldict = {}

    Graph = nx.DiGraph()
    # Graph.add_node("genesis")
    # labeldict["genesis"] = "genesis"
    # Graph.add_node("00000000000000")
    # labeldict["00000000000000"] = "0000"
    # Graph.add_edge("genesis", "00000000000000")

    Graph.add_node("00000000000000")
    labeldict["00000000000000"] = "genesis"


    pattern = re.compile(r"Block idx=([0-9]+),.*,Block hash=\[\[([0-9a-fA-F]{14}).+\]\[.+\]\],Prev Block hash=\[\[([0-9a-fA-F]{14}).+\]\[.+\]\],Timestamp=\[(.+)\],Difficulty=.+")

    for i in range(1, nodenum):
        logfile = "./%s/hosts/bleep%d/stdout-bleep%d.%s.1000.log" % (datadir, i, i, shadow_plugin)

        blocks = {}
        f = open(logfile)
        for line in f:
            match = pattern.search(line)
            if match:
                block_idx = int(match.group(1))
                block_hash =  match.group(2)
                prev_block_hash = match.group(3)
                timestamp = float(match.group(4))
                blocks[block_idx] = (block_hash, prev_block_hash, timestamp)
                # if not Graph.has_node(block_hash):
                #     Graph.add_node(block_hash)
                # if not Graph.has_node(prev_block_hash):
                #     Graph.add_node(prev_block_hash)
                # Graph.add_edge(prev_block_hash, block_hash)
        for idx in sorted(blocks.keys()):
            (block_hash, prev_block_hash, timestamp) = blocks[idx]
            if not Graph.has_node(block_hash):
                Graph.add_node(block_hash)
                labeldict[block_hash] = block_hash[:4]
                labeldict[block_hash] = str(timestamp)
            if not Graph.has_node(prev_block_hash):
                Graph.add_node(prev_block_hash)
                labeldict[prev_block_hash] = prev_block_hash[:4]
                labeldict[block_hash] = timestamp
            Graph.add_edge(prev_block_hash, block_hash)

        # print blocks
        # print sorted(blocks.keys())
        if len(blocks) > 0:
            (block_hash, prev_block_hash, timestamp) = blocks[sorted(blocks.keys())[-1]]
            bleep_id = "bleep%d" % i
            Graph.add_node(bleep_id)
            labeldict[bleep_id] = bleep_id
            Graph.add_edge(block_hash, bleep_id)


    # pos = hierarchy_pos(Graph, "genesis")
    pos = hierarchy_pos(Graph, "00000000000000")

    # labeldict = {}
    nx.draw(Graph, pos=pos, labels=labeldict, with_labels = True, width=2, node_size = 200, node_shape = 's', arrowstyle='simple', arrowsize=1, node_color = 'white')
    plt.show()
