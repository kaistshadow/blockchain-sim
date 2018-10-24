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
    if len (sys.argv) != 2 :
        print "Command line input error"
        sys.exit (1)

    nodenum = int(sys.argv[1])
    datadir = "pow-consensus-datadir"
    # shadow_plugin = "PEER_POWCONSENSUS"
    # datadir = "centralized-broadcast-datadir"
    # datadir = "pow-consensus-large-datadir"
    shadow_plugin = "PEER_POWCONSENSUS"

    Graph = nx.DiGraph()
    Graph.add_node("genesis")
    Graph.add_node("0000000")
    Graph.add_edge("genesis", "0000000")

    pattern = re.compile(r"Block idx=([0-9]+),.*,Block hash=\[\[([0-9a-fA-F]{7}).+\]\[.+\]\],Prev Block hash=\[\[([0-9a-fA-F]{7}).+")

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
                blocks[block_idx] = (block_hash, prev_block_hash)
                # if not Graph.has_node(block_hash):
                #     Graph.add_node(block_hash)
                # if not Graph.has_node(prev_block_hash):
                #     Graph.add_node(prev_block_hash)
                # Graph.add_edge(prev_block_hash, block_hash)
        for idx in sorted(blocks.keys()):
            (block_hash, prev_block_hash) = blocks[idx]
            if not Graph.has_node(block_hash):
                Graph.add_node(block_hash)
            if not Graph.has_node(prev_block_hash):
                Graph.add_node(prev_block_hash)
            Graph.add_edge(prev_block_hash, block_hash)

        # print blocks
        # print sorted(blocks.keys())
        (block_hash, prev_block_hash) = blocks[sorted(blocks.keys())[-1]]
        bleep_id = "bleep%d" % i
        Graph.add_node(bleep_id)
        Graph.add_edge(block_hash, bleep_id)


    pos = hierarchy_pos(Graph, "genesis")

    nx.draw(Graph, pos=pos, with_labels = True, width=2, node_size = 1000, node_shape = 's', arrowstyle='simple', node_color = 'white')
    plt.show()
