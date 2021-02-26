import os
from subprocess import check_output
import argparse
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
import test_modules

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def test_args(args_standard):
    for i in range(0,len(args_standard)):
        if args_standard[i] is None:
            sys.exit(1)

def get_plugin_args(plugin_infos):
    result_list = []
    f = open(plugin_infos, "r")
    while True:
        line = f.readline()
        if not line:break
        result = line.find('plugin="bitcoind"')
        if result != -1:
            split_list = line.split("arguments=")
            break
    
    split_list = split_list[1].split(" ")
    for i in range(0,len(split_list)):
        result = split_list[i].find("port")
        if result != -1:
            result_list.append(split_list[i].split("=")[1])
        result = split_list[i].find("datadir")
        if result != -1:
            result_list.append(split_list[i].split("=")[1])

    print(result_list)
    return result_list

# This test compares the shadow result log with standard args, 
# and succeeds if all of the args are in the shadow result log.    
def test_bitcoinApplication(output_file, args_standard):
    j = 0
    f = open(output_file[0], "r")
    while True:
        line = f.readline()
        if not line: break
        for i in range(j,len(args_standard)):
            result = line.find(args_standard[i])
            if result != -1:
                j += 1
    f.close()
    if j == len(args_standard):
        print("Success bitcoin application test ...")
        sys.exit(0)
    else:
        print("Fail bitcoin application test ...")
        sys.exit(1)

def main():

    # xml 파일이 생성될 위치를 현재위치로 설정
    path = os.path.abspath("./")

    # xml 파일 생성
    test_modules.get_xmlfile(path)

    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("output.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = test_modules.get_xml_info_new(target_folder_xml)

    # plugin arguments 가져오기.
    args_standard = get_plugin_args(target_folder_xml)

    # 지금 예제는 transaction injector를 사용 안하기에 별도의 xml파일을 만들어줌.
    # 기존 xml에는 transaction.so에 대한 정의가 있어서, 이를 삭제안해주면 shadow가 실행이 안됨.
    test_modules.set_plugin_file(len(node_id_list), path)
    test_modules.remove_tx_plugin(target_folder_xml)
    target_folder_xml = target_folder_xml[:len(target_folder_xml)-4] + "2.xml"
    shadow_command = "shadow " + target_folder_xml

    # shadow 실행
    print("shadow running ...")
    exec_shell_cmd(shadow_command)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # bitcoin application test 실행.
    test_bitcoinApplication(simulation_output_file ,args_standard)

if __name__ == '__main__':
    main()