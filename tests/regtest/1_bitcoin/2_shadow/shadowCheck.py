import os
import subprocess
import argparse
import sys
import shlex
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from libraries import test_modules

# Test process
# 1. Test - xml file existence 
# 2. Run shadow and get shadow.output
# 3. Get infos such as simulation runtime, plugin id from xml file
# 4. Test - shadow output file existence
# 5. Test - shadow
def main():

    # xml 파일이 생성될 위치를 현재위치로 설정
    path = os.path.abspath("./")

    # xml 파일 생성
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
    target_folder_file = test_modules.test_shadow_output_file_existence()

    # shadow output 파일 내용 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list)

    # 성공하면
    sys.exit(0)

if __name__ == '__main__':
    main()