import os
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)
    
def main():
    node_count = sys.argv[1]
    difficulty = sys.argv[2]
    for i in range(0,int(node_count)):
        command = "mkdir -p data/bcdnode" + str(i)
        print(command)
        exec_shell_cmd(command)
        command = "cp -r ../../../testlibs/dump/difficulty_" + difficulty + "/bcdnode0/* ./data/bcdnode" + str(i)
        exec_shell_cmd(command)
    
    path = "../../../testlibs/dump/difficulty_" + difficulty

    command = "cp -r " + path + "/coinflip_hash.txt ./data"
    exec_shell_cmd(command)
    command = "cp -r " + path + "/key.txt ./data"
    exec_shell_cmd(command)
    command = "cp -r " + path + "/state.txt ./data"
    exec_shell_cmd(command)


if __name__ == '__main__':
    main()
    
