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

# check "getnewaddress" log in shadow output file. If done, check length of rpc_file contents. If the length is 35, the test is success.
def test_walletAddress(simulation_output_file):
    return_count = 0
    f = open(simulation_output_file[0], "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("getnewaddress")
        if result != -1:
            return_count += 1
    f.close()
    if return_count == 0:
        print("[Fail wallet test] - bitcoind didn't works about getnewaddress")
        sys.exit(1)

    f = open(simulation_output_file[1], "r")
    line = f.readline().strip()
    f.close()
    if len(line) == 35:
        print("Success wallet test ...")
        sys.exit(0)
    else:
        print("[Fail wallet test] - wallet address length is not 35")
        sys.exit(1)

# test process
# 1. test_xml_existence 
# 2. shadow output existence test
# 3. wallet log test
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
    exec_shell_cmd(shadow_command)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # wallet test 시작.
    test_walletAddress(simulation_output_file)

if __name__ == '__main__':
    main()