import os
from subprocess import check_output
import argparse
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from libraries import test_modules

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

    # xml 파일 생성
    # tx_mode = test_modules.get_xmlfile(path)

    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("output.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = test_modules.get_xml_info_new(target_folder_xml)

    # bitcoind data dir 설정 파일 생성.
    test_modules.set_plugin_file(len(node_id_list), path)

    # 지금 예제는 transaction injector를 사용 안하기에 별도의 xml파일을 만들어줌.
    # 기존 xml에는 transaction.so에 대한 정의가 있어서, 이를 삭제안해주면 shadow가 실행이 안됨.
    # shadow_command = test_modules.renew_xml(tx_mode, target_folder_xml)

    # shadow 실행
    print("shadow running ...")
    test_modules.subprocess_open('shadow output2.xml > output.txt')

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # Initial coin test 실행.
    test_initialCoin(simulation_output_file[1])
   

if __name__ == '__main__':
    main()
def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)
    