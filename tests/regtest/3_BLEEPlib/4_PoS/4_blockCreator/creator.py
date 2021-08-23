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

def get_ratio_stake(txt_file):
    stake_ratio = []
    f = open(txt_file, "r")
    while True:
        line = f.readline()
        if not line: break
        stake_ratio.append(line[:-1].split(":")[1])
        
    f.close()
    return stake_ratio

#PoS Debug - random selection for slot#:21038418|Debug Selected:1
def get_ratio_fromOutput(shadow_output_log, stake_ratio):
    stake_list = []
    return_list = []

    for i in range(0,len(stake_ratio)):
        blank_list = []
        stake_list.append(blank_list)

    f = open(shadow_output_log, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("Debug Selected:")
        if result != -1:
            slot = int(line.split(":")[1].split("|")[0])
            index = int(line.split(":")[2][:-1])
            stake_list[index].append(slot)
            
    f.close()
    for i in range(0,len(stake_list)):
        return_list.append(len(set(stake_list[i])))
    return return_list

def test_blockCreator(path):
    
    count = 0
    # Run shadow
    print("Start POS block create ratio simulation ...")
    exec_shell_cmd("shadow example.xml > /dev/null 2>&1")
    print("Finish POS block create ratio simulation ...")

    # Get stake ratio
    txt_file = path + "/" + "stakefile.txt"
    stake_ratio = get_ratio_stake(txt_file)

    # Get total process about creating block and who created
    shadow_output_log = path + "/shadow.data/hosts"
    shadow_output_log = shadow_output_log + "/" + os.listdir(shadow_output_log)[0]
    shadow_output_log = shadow_output_log + "/" + os.listdir(shadow_output_log)[0]
    return_list = get_ratio_fromOutput(shadow_output_log, stake_ratio)
    
    # test ratio
    for i in range(0,len(return_list)):
        return_id = return_list.index(max(return_list))
        txt_id = stake_ratio.index(max(stake_ratio))
        if return_id == txt_id:
            count += 1

    if count == len(return_list):
        print("Success block creator ratio test ...")
        sys.exit(0)
    else:
        print("Fail block creator ratio test ...")
        sys.exit(1)
    

def main():
    path = os.path.abspath("./")
    test_blockCreator(path)

if __name__ == '__main__':
    main()