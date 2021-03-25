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
    
    if OPT_REGTEST:
        xml_modules.get_xmlfile(path)

    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("output.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = xml_modules.get_xml_info_new(target_folder_xml)

    # bitcoind data dir 설정 파일 생성.
    target_path = path + "/data"
    utils.set_plugin_file(len(node_id_list), target_path)
    # shadow 실행
    print("shadow running ...")
    shadow_command = 'shadow output.xml -w 2' + ' > output.txt'
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

    # transaction count test 실행.
    test_modules.test_transaction_count_regtest(simulation_output_file, int(len(plugin_list)/2))


if __name__ == '__main__':
    main()