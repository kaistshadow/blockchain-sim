#
# 2021-06-15
# created by hong joon
#
import os
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def test_genesis_diffilculty(target_list, target_path):
    each_node_genesisList = []
    for i in range(0,len(target_list)):
        target_where = target_path + "/" + target_list[i]
        target_output_file = os.listdir(target_where)
        target_output_file = target_where + "/" + target_output_file[0]
        f = open(target_output_file, "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find("genesis blk generated and its hash")
            if result != -1:
                each_node_genesisList.append(line)
                break
        f.close()
    each_node_genesisList = set(each_node_genesisList)
    if len(each_node_genesisList) == 1:
        print("Successfully BLEEP pos genesis regression test ...")
        sys.exit(0)
    else:
        print("Fail BLEEP pos genesis regression test ...")
        sys.exit(1)

def main():
    path = os.path.abspath("./")
    print("Start BLEEP pos genesis regression test ...")
    exec_shell_cmd("shadow example.xml > /dev/null 2>&1")
    target_path = path + "/shadow.data/hosts"
    target_list = os.listdir(target_path)
    test_genesis_diffilculty(target_list, target_path)

if __name__ == '__main__':
    main()