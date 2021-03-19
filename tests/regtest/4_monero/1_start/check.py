import sys
import os

sys.path.append("./../../../../")
from testlibs import test_modules


def check_verack(monero_log):
    f = open(monero_log, "r")
    iterf = iter(f)
    check_flag = [False, False, False, False, False]
    for line in iterf:
        # check whether the Monero's first message is printed
        if "Monero 'Oxygen Orion'" in line:
            check_flag[0] = True
            print("CHECK : Monero Initiated")

        # check whether the cryptonote protocol is initialized
        if check_flag[0] and "Cryptonote protocol initialized OK" in line:
            check_flag[1] = True
            print("CHECK : Cryptonote initialized")

        # check whether the Core is initialized
        if check_flag[1] and "Core initialized OK" in line:
            check_flag[2] = True
            print("CHECK : Core initialized OK")

        # check whether the p2p server is initialized
        if check_flag[2] and "p2p server initialized OK" in line:
            check_flag[3] = True
            print("CHECK : p2p server initialized OK")

        # check whether the daemon message is printed
        if check_flag[3] and "The daemon will start synchronizing with the network" in line:
            check_flag[4] = True
            print("CHECK : daemon message is printed")

    if all(check_flag):
        print("Success checking output results of the experiment(Monero start) ...")
        return
    else:
        print("Fail checking output results of the experiment(Monero start) ...")
        exit(-1)


def main(shadowpath):
    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("topology.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = test_modules.get_xml_info_new(target_folder_xml)

    # shadow 실행
    print("shadow running ...")
    test_modules.subprocess_open('%s topology.xml > output.txt' % shadowpath)

    # shadow output 파일 존재 검사.
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest")

    # shadow output 파일 내용 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # Monero 실험 결과 체크 시작.
    check_verack(simulation_output_file[0])


if __name__ == '__main__':
    main(sys.argv[1])
