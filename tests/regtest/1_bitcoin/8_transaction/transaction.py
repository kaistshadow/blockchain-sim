#
# 2021-03-18
# created by hong joon
#
import os
from subprocess import check_output
import argparse
import sys
sys.path.append("./../../../../")
from testlibs import test_modules, utils, xml_modules

# Test process
# 1. test_xml_existence 
# 2. shadow output existence test  
# 3. Run test_transaction_existence function
def main():

    # xml 파일이 생성될 위치를 현재위치로 설정
    path = os.path.abspath("./")

    parser = argparse.ArgumentParser(description='Script to reset xml file')
    parser.add_argument("--regtest", action="store_true", help="Make xml file.")
    args = parser.parse_args()
    OPT_REGTEST = args.regtest
    difficulty = ""

    if OPT_REGTEST:
        tx_mode, algo, difficulty = xml_modules.get_xmlfile(path)
    else:
        utils.exec_shell_cmd("sh clean_data.sh 10")
        present_path = path + "/output.xml"
        difficulty = utils.get_difficulty_fromXML(present_path)

    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("output.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = xml_modules.get_xml_info_new(target_folder_xml)

    # bitcoind data dir 설정 파일 생성.
    target_path = path + "/data"
    # shadow 실행
    print("shadow running ...")
    shadow_command = 'shadow output.xml > output.txt'
    utils.subprocess_open(shadow_command)
    output_file = path + "/output.txt" 

    # shadow output 파일 존재 검사.
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest", node_id_list)

    # shadow output 파일 내용 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list, output_file)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # wallet test 시작.
    test_modules.test_walletAddress(simulation_output_file, int(len(plugin_list)/2))
    
    # transaction test 시작.
    test_modules.test_transaction_existence(simulation_output_file, int(len(plugin_list)/2))
    sys.exit(0)

if __name__ == '__main__':
    main()