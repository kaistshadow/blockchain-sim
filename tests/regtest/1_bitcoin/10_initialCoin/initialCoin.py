import os
from subprocess import check_output
import argparse
import sys
sys.path.append("./../../../../")
from testlibs import test_modules, utils, xml_modules

def test_initialCoin(simulation_output_file):
    f = open(simulation_output_file, "r")
    block_count = 0
    
    for line in f.readlines()[::-1]: 
        
        result = line.find("bestblockhash")
        if result != -1:
            f.close()
            split_list = line.split(",")
            for i in range(0,len(split_list)):
                result = split_list[i].find("blocks")
                if result != -1:
                    block_count = int(split_list[i].split(":")[1])
                    break
        if block_count != 0:
            f.close()
            break
            
    if block_count > 0:
        print("block_count : %d" %block_count)
        print("Success InitalCoin test ...")
        sys.exit(0)
    else:
        print("block_count : %d" %block_count)
        print("Fail InitalCoin test ...")
        sys.exit(1)

# test process
# 1. test_xml_existence 
# 2. shadow output existence test
# 3. wallet log test
def main():

    # xml 파일이 생성될 위치를 현재위치로 설정
    path = os.path.abspath("./")

    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--regtest", action="store_true", help="Install the shadow simulator and BLEEP")
    args = parser.parse_args()
    OPT_REGTEST = args.regtest
    
    if OPT_REGTEST:
        xml_modules.get_xmlfile(path)

    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("output2.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = xml_modules.get_xml_info_new(target_folder_xml)

    # bitcoind data dir 설정 파일 생성.
    utils.set_plugin_file(len(node_id_list), path)

    # shadow 실행
    print("shadow running ...")
    utils.subprocess_open('shadow output2.xml > output.txt')
    output_file = path + "/output.txt" 

    # shadow output test
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest", node_id_list)

    # shadow output 파일 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list, output_file)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # wallet test 시작.
    test_modules.test_walletAddress(simulation_output_file)
    
    # Initial coin test 실행.
    test_initialCoin(simulation_output_file[1])
   

if __name__ == '__main__':
    main()
def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)
    