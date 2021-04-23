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

def main():

    # xml 파일이 생성될 위치를 현재위치로 설정
    path = os.path.abspath("./")

    parser = argparse.ArgumentParser(description='Script to reset xml file')
    parser.add_argument("--regtest", action="store_true", help="Make xml file.")
    args = parser.parse_args()
    OPT_REGTEST = args.regtest
    difficulty = ""
    present_path = ""

    if OPT_REGTEST:
        tx_mode, algo, difficulty = xml_modules.get_xmlfile(path)
    else:
        utils.exec_shell_cmd("sh clean_data.sh 2")
        present_path = path + "/test-BitcoinP2P.xml"
        difficulty = utils.get_difficulty_fromXML(present_path)
        utils.check_dump_data(difficulty)
        
    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("test-BitcoinP2P.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = xml_modules.get_xml_info_new(target_folder_xml)

    # bitcoind data dir 설정 파일 생성.
    target_path = path + "/data"

    # shadow 실행
    print("shadow running ...")
    utils.subprocess_open('shadow test-BitcoinP2P.xml > output.txt')
    output_file = path + "/output.txt" 

    # shadow output test
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest", node_id_list)

    # shadow output 파일 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list, output_file)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # # monitor node connection test
    # parameter3 : node의 개수에서 -4 의 의미는 (client * 2) + (txGenerator) + (monitor) 이렇게 빼줘야하기 때문.
    test_modules.TxGenerator_connection_test(simulation_output_file, present_path, (len(node_id_list)-4))

if __name__ == '__main__':
    main()
    
def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)
    