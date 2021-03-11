import os
from subprocess import check_output
import argparse
import sys
sys.path.append("./../../../../")
from testlibs import test_modules

# Get "bestblockchash" value info using rpc.output_file then check for the same value in bitcoin output log.
def test_MainChainInfo(shadow_output_file, rpc_output_file):
    return_count = 0
    f = open(rpc_output_file , "r")
    for line in f.readlines()[::-1]:
        line.rstrip()
        result = line.find("bestblockhash")
        if result != -1:
            line = line.split(",")[3].split(":")[1]
            genesisHash = line[1:len(line)-1]
            return_count = 1
            break

    if return_count == 0:
        sys.exit(1)
    if os.path.isfile(shadow_output_file):
        f = open(shadow_output_file, "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find(genesisHash)
            if result != -1:
                print("Success MainChain test ...")
                sys.exit(0)
    else:
        print("Fail MainChain test ...")
        sys.exit(1)

    
# Test process
# 1. test_xml_existence 
# 2. shadow output existence test  
# 3. Run test_MainChainInfo function
def main():

    # xml 파일이 생성될 위치를 현재위치로 설정
    path = os.path.abspath("./")

    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--regtest", action="store_true", help="Install the shadow simulator and BLEEP")
    args = parser.parse_args()
    OPT_REGTEST = args.regtest
    
    if OPT_REGTEST:
        test_modules.get_xmlfile(path)

    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("output.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = test_modules.get_xml_info_new(target_folder_xml)

    # 지금 예제는 transaction injector를 사용 안하기에 별도의 xml파일을 만들어줌.
    # 기존 xml에는 transaction.so에 대한 정의가 있어서, 이를 삭제안해주면 shadow가 실행이 안됨.
    test_modules.set_plugin_file(len(node_id_list), path)
    test_modules.remove_tx_plugin(target_folder_xml)
    target_folder_xml = target_folder_xml[:len(target_folder_xml)-4] + "2.xml"
    shadow_command = "shadow " + target_folder_xml

    # shadow 실행
    print("shadow running ...")
    test_modules.subprocess_open('shadow output2.xml > output.txt')

    # shadow output 파일 존재 검사.
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest")

    # shadow output 파일 내용 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # mainchain test 시작.
    test_MainChainInfo(simulation_output_file[0], simulation_output_file[1])
    

if __name__ == '__main__':
    main()