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

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def main():

    # xml 파일이 생성될 위치를 현재위치로 설정
    path = os.path.abspath("./")

    # Start TPS test 
    exec_shell_cmd("sh start_test.sh 3 TPS")

    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("test-BitcoinTPS.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = xml_modules.get_xml_info_new(target_folder_xml)

    # shadow output test
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest", node_id_list)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # TPS test


if __name__ == '__main__':
    main()

    