import os
import subprocess
import argparse
import sys
import time
sys.path.append('../../')
from testlibs import test_modules, utils, xml_modules, test_result

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def main():

    path = os.path.abspath("./")

    # xml file 생성
    tx_mode, algo = xml_modules.get_xmlfile(path)

    # # xml file 생성 검증
    test_modules.test_xml_existence("output.xml")
 
    # # xml file에서 plugin, simulation time 정보 추출
    runtime, node_id_list, plugin_list = xml_modules.get_xml_info_new("output.xml")
    output_file = path + "/output.txt" 

    # IP address 가져오기
    IP_list = utils.get_address_list("output.xml")

    # # datadir 설정
    utils.set_plugin_file(len(node_id_list), os.path.abspath("./data"))

    # # shadow 실행
    print("shadow running ... ")
    utils.subprocess_open('shadow output.xml > output.txt')
    exec_shell_cmd('mv output.txt shadow.data')

    # # shadow output file 생성 여부 검증
    target_folder_file = test_modules.test_shadow_output_file_existence("emulation", node_id_list)

    # # shadow output file이 제대로 생성된지 검증
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    if tx_mode == "enable":
        # # Tx가 제대로 생성된지 검증.
        test_modules.test_transaction_existence(simulation_output_file, int(len(plugin_list)/2))

    # # shadow output 검증
    complete_node, runtime = test_modules.emul_test_shadow(target_folder_file, runtime, node_id_list, output_file)
    
    # wallet test
    test_modules.test_walletAddress(simulation_output_file, int(len(plugin_list)/2))

    # result 
    output_txt = path + "/shadow.data/output.txt"
    test_result.summary_result(complete_node, simulation_output_file, runtime, output_txt, algo)

    print("emulation success!!")
    sys.exit(0)

if __name__ == '__main__':
    main()