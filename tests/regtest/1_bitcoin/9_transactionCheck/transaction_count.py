import os
from subprocess import check_output
import argparse
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
from libraries import test_modules

# count "sendtoaddress" rpc request in bitcoin log and get transaction counts in tx injector log.
# If the two are the same, true
def test_transaction_count(simulation_output_file):
    txs_bitcoind = 0
    blocks_count = 0
    mempool_size = 0
    f = open(simulation_output_file[0], "r")
    for line in f.readlines()[::-1]:
        result = line.find("UpdateTip")
        if result != -1:
            split_list = line.split(" ")
            for i in range(0,len(split_list)):
                result = split_list[i].find("height=")
                if result != -1:
                    blocks_count = int(split_list[i].split("=")[1])
                    continue
                result = split_list[i].find("tx=")
                if result != -1:
                    txs_bitcoind = int(split_list[i].split("=")[1])
                    break
            if txs_bitcoind != 0:
                break    
    f.close()
    txs_bitcoind = txs_bitcoind - blocks_count - 1

    txs = 0
    f = open(simulation_output_file[2], "r")
    for line in f.readlines()[::-1]:
        result = line.find('"error":null')
        if result != -1:
            txs += 1
        result = line.find('"result":null')
        if result != -1:
            break

    f = open(simulation_output_file[1], "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("maxmempool")
        if result != -1:
            mempool_size = line.split(",")[1].split(":")[1]
            break
    f.close()

    if txs_bitcoind + int(mempool_size) == txs:
        print("Sucess transaction count test ... ")
        sys.exit(0)
    else:
        print("")
        print("--------------------------------------")
        print("txs_bitcoind : %d" %txs_bitcoind)
        print("mempool_size : %s" %mempool_size)
        print("txs : %d" %txs)
        print("Fail transaction count test ...")
        sys.exit(1)


# Test process
# 1. test_xml_existence 
# 2. shadow output existence test  
# 3. Run test_transaction_existence function
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

    # bitcoind data dir 설정 파일 생성.
    test_modules.set_plugin_file(len(node_id_list), path)

    # 지금 예제는 transaction injector를 사용 안하기에 별도의 xml파일을 만들어줌.
    # 기존 xml에는 transaction.so에 대한 정의가 있어서, 이를 삭제안해주면 shadow가 실행이 안됨.
    # shadow_command = test_modules.renew_xml(tx_mode, target_folder_xml)

    # shadow 실행
    print("shadow running ...")
    shadow_command = 'shadow output.xml -w 2' + ' > output.txt'
    test_modules.subprocess_open(shadow_command)

    # shadow output 파일 존재 검사.
    target_folder_file = test_modules.test_shadow_output_file_existence()

    # shadow output 파일 내용 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)
    
    # transaction count test 실행.
    test_transaction_count(simulation_output_file)

if __name__ == '__main__':
    main()