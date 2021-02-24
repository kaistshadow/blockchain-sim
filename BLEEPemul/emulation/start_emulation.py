import os
import subprocess
import argparse
import sys
sys.path.append(os.path.dirname(os.path.abspath(os.path.dirname(__file__))))
import testmodule

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def count_block(node_output_file):
    block_count = 0
    f = open(node_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("UpdateTip")
        if result != -1:
            block_count += 1
    f.close()
    return block_count

def get_last_blockHash(node_output_file):
    f = open(node_output_file, "r")
    for line in f.readlines()[::-1]:
        result = line.find("UpdateTip")
        if result != -1:
            split_data = line.split(" ")[3].split("=")[1]
            break
    f.close()
    return split_data

def get_blockHash_list(node_output_file):
    block_hash_list = []
    f = open(node_output_file, "r")
    while True:
        line = f.readline()
        if not line: break
        result = line.find("UpdateTip")
        if result != -1:
            block_hash_list.append(line.split(" ")[3].split("=")[1])
    f.close()
    return block_hash_list

def summary_result(node_list, node_output_file, sim_time):
    txs = testmodule.test_transaction_count(node_output_file)
    print("---------------------------------------------------------------------------------")
    print("1. node 갯수 : %d" %len(node_list))
    print("2. simulation time : %s sec" %sim_time)
    for i in range(0,len(node_list)):
        print("3.  ")
        print("\t3-%d IP address : %s" %(i+1, node_list[i]))
    block_count = count_block(node_output_file[0])
    print("4. 생성된 블록 개수 : %d" %block_count)
    print("5. 마지막 블록의 hash 값 : %s" %get_last_blockHash(node_output_file[0]))
    print("6. 생성된 트랜잭션 개수 : %d" %txs)
    print("7. TPS : %s" %(txs/int(sim_time)))
    print("---------------------------------------------------------------------------------")
    print("8. Blockhash list")
    block_hash_list = get_blockHash_list(node_output_file[0])
    for i in range(0,len(block_hash_list)):
        print("%d blockhash : %s" %((i+1), block_hash_list[i]))

def main():

    # xml file 생성
    testmodule.get_xmlfile()

    # # xml file 생성 검증
    # target_folder_xml = testmodule.test_xml_existence("output.xml")

    # # shadow 실행
    # print("shadow running ... ")
    # testmodule.subprocess_open('shadow output.xml > output.txt')

    # # shadow output file 생성 여부 검증
    # target_folder_file = testmodule.test_shadow_output_file_existence()

    # # xml file에서 plugin, simulation time 정보 추출
    # runtime, node_id_list, plugin_list = testmodule.get_xml_info("output.xml")

    # # shadow output file이 제대로 생성된지 검증
    # simulation_output_file = testmodule.test_file_existence(node_id_list, plugin_list)

    # # Tx가 제대로 생성된지 검증.
    # testmodule.test_transaction_existence(simulation_output_file[1])

    # # shadow output 검증
    # complete_node, runtime = testmodule.test_shadow(target_folder_file, runtime, node_id_list)
    
    # # result summary
    # summary_result(complete_node, simulation_output_file, runtime)
    
    # sys.exit(0)

if __name__ == '__main__':
    main()