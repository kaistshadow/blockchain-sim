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
#leader:1|stakedValue:60
def get_stake_shadow_output(shadow_output_log):
    list_data = []
    f = open(shadow_output_log, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("leader:")
        if result != -1:
            nodeID = line.split("|")[0].split(":")[1]
            stake = line.split("|")[1].split(":")[1][:-1]
            input_data = nodeID + ":" + stake
            list_data.append(input_data)
    f.close()
    return list_data

def get_stake_form(txt_file):
    stake_list = []
    f = open(txt_file, "r")
    while True:
        line = f.readline()
        if not line: break
        stake_list.append(line[:-1])
    f.close()
    return stake_list

def test_stake(path):
    # running shadow
    print("Start POS stake test ...")
    # exec_shell_cmd("shadow example.xml > /dev/null 2>&1")
    print("Finish POS stake test ...")

    # get stake value from txt file
    txt_file = path + "/stakefile.txt"
    stake_list = get_stake_form(txt_file)

    # get stake value from shadow log
    shadow_output_log = path + "/shadow.data/hosts"
    shadow_output_log = shadow_output_log + "/" + os.listdir(shadow_output_log)[0]
    shadow_output_log = shadow_output_log + "/" + os.listdir(shadow_output_log)[0]
    list_data = get_stake_shadow_output(shadow_output_log)

    # compare txt_file with stake_list
    if list_data == stake_list:
        print("Success POS stake test ...")
        sys.exit(0)
    else:
        print("Fail POS stake test ...")
        sys.exit(1)

def main():
    path = os.path.abspath("./")
    test_stake(path)


if __name__ == '__main__':
    main()