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

def getPeerAddress_FromXML(xmlfile):
    f = open(xmlfile, "r")
    connect_peerlist = []
    while True:
        line = f.readline()
        if not line: break
        result = line.find("application")
        if result != -1:
            split_list = line.split(" ")
            for i in range(0,len(split_list)):
                result = split_list[i].find("connect")
                if result != -1:
                    connect_peerlist.append(split_list[i].split("=")[1][:-4])

    f.close()
    return connect_peerlist

def test_peerConnection(shadow_outputpath, connect_peerlist):
    test_condition = 0
    target_list = os.listdir(shadow_outputpath)
    for i in range(0,len(target_list)):
        target_output = shadow_outputpath + "/" + target_list[i]
        target_output_file = os.listdir(target_output)
        target_output = target_output + "/" + target_output_file[0]
        f = open(target_output, "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find(connect_peerlist[i])
            if result != -1:
                test_condition += 1
        f.close()

    if test_condition == 2:
        print("Success POS peerConnection test ...")
        sys.exit(0)
    else:
        print("Fail POS peerConnection test ...")
        sys.exit(1)
    

def main():
    path = os.path.abspath("./")
    xmlfile = path + "/example.xml"
    print("Start POS peerConnection test ...")
    exec_shell_cmd("shadow example.xml > /dev/null 2>&1")
    print("Success POS simulation ...")
    connect_peerlist = getPeerAddress_FromXML(xmlfile)
    shadow_outputfile = path + "/shadow.data/hosts"
    test_peerConnection(shadow_outputfile, connect_peerlist)

if __name__ == '__main__':
    main()