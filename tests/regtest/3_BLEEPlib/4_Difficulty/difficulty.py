#
# 2021-03-18
# created by hong joon
#
import os
import subprocess
import sys
sys.path.append("./../../../../")
from testlibs import test_modules, utils, xml_modules, bleep_regtest_module

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def main():

    exec_shell_cmd("rm -rf shadow.data")

    # xml 파일이 생성될 위치를 현재위치로 설정
    path = os.path.abspath("./")

    target_folder_xml = "treesynctest1-twonode.xml"

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = xml_modules.get_xml_info_new(target_folder_xml)

    # shadow 실행
    print("shadow running ...")
    utils.subprocess_open('shadow treesynctest1-twonode.xml > output.txt')
    output_file = path + "/output.txt" 

    # shadow output 파일 존재 검사.
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest", node_id_list)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # difficulty test 시작.
    bleep_regtest_module.test_difficulty(simulation_output_file)

if __name__ == '__main__':
    main()