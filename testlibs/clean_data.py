import os
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)
    
def main():
    node_count = sys.argv[1]
    difficulty = sys.argv[2]
    my_path = sys.argv[3]

    try:
        command = "cd " + my_path + "; rm -rf data"
        exec_shell_cmd(command)
    except:
        pass

    for i in range(0,int(node_count)):
        command = "cd " + my_path + "; mkdir -p data/bcdnode" + str(i)
        exec_shell_cmd(command)
        command = "cp -r ./dump/difficulty_" + difficulty + "/bcdnode0/* " + my_path + "/data/bcdnode" + str(i)
        exec_shell_cmd(command)
    
    path = "./dump/difficulty_" + difficulty

    command = "cp -r " + path + "/coinflip_hash.txt " + my_path + "/data"
    exec_shell_cmd(command)
    command = "cp -r " + path + "/key.txt " + my_path + "/data"
    exec_shell_cmd(command)
    command = "cp -r " + path + "/state.txt " + my_path + "/data"
    exec_shell_cmd(command)


if __name__ == '__main__':
    main()
    
