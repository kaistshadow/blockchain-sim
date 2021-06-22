#
# 2021-06-15
# created by hong joon
#
import os
import sys
import pos_xmlGenerator

def main():
    print("Start genesis py ...")
    path = os.path.abspath("./")
    node_count = int(sys.argv[1])
    simulation_time = int(sys.argv[2])

    pos_xmlGenerator.setup_multiple_node_xml(node_count, simulation_time)
    print("Finish")

if __name__ == '__main__':
    main()