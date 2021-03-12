import os
import subprocess
import argparse
import sys
import shlex
sys.path.append("./../../../../")
from testlibs import test_modules

def get_difficulty_info(xml_file):
    count = 0
    f = open(xml_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("difficulty")
        if result != -1:
            split_list = line.split(' ')
            for i in range(0,len(split_list)):
                result = split_list[i].find("difficulty")
                if result != -1:
                    for i in range(0,len(split_list)):
                        result = split_list[i].find("difficulty")
                        if result != -1:
                            difficulty = split_list[i].split("=")[1]
                            break
                    break
        if count != 0:
            break
    f.close()
    return difficulty[0]

def test_difficulty_compare(bitcoin_log, xml_difficulty):

    difficulty = ""
    f = open(bitcoin_log, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("difficulty")
        if result != -1:
            split_list = line.split(",")[4]
            difficulty = split_list.split(":")[1]
            break
    f.close()

    if str(xml_difficulty) == "1":
        if difficulty == "1":
            print("Success difficulty test ...")
            sys.exit(0)
        else:
            print("Fail difficulty test ...")
            sys.exit(1)

    elif str(xml_difficulty) == "2":
        if difficulty == "0.00390625":
            print("Success difficulty test ...")
            sys.exit(0)
        else:
            print("Fail difficulty test ...")
            sys.exit(1)
        # error case                                

    elif str(xml_difficulty) == "3":
        if difficulty == "0.0002441371325370145":
            print("Success difficulty test ...")
            sys.exit(0)
        else:
            print("Fail difficulty test ...")
            sys.exit(1)

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
    output_file = path + "/output.txt" 

    # shadow output 파일 존재 검사.
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest", node_id_list)

    # shadow output 파일 내용 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list, output_file)
    
    # xml로 부터 difficulty 정보 얻어옴.
    xml_difficulty = get_difficulty_info(target_folder_xml)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # difficulty test 시작.
    test_difficulty_compare(simulation_output_file[1], xml_difficulty)

if __name__ == '__main__':
    main()