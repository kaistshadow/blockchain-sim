import sys
import os

sys.path.append('../../../../')
from testlibs import test_modules, utils, xml_modules


def check_miniEREBUS(bitcoin_log, tester_log):
    f = open(tester_log, "r")
    iterf = iter(f)
    check_flag = [False, False, False]
    for line in iterf:
        # check whether the connection is tried
        if not check_flag[0] and "outgoing connection is confirmed" in line:
            check_flag[0] = True
            print("CHECK : Intercept 1st outgoing connection")
            continue

        # check whether the connection is established
        if check_flag[0] and "outgoing connection is confirmed" in line:
            check_flag[1] = True
            print("CHECK : Intercept 2nd outgoing connection")

        # check whether the version message is sent
        if check_flag[1] and "The EREBUS attack is succeeded." in line:
            check_flag[2] = True
            print("CHECK : EREBUS attack success message confirmed")

    if all(check_flag):
        print("Success checking output results of the experiment(miniEREBUS) ...")
        return
    else:
        print("Fail checking output results of the experiment(miniEREBUS) ...")
        exit(-1)


def main(shadowpath):

    path = os.path.abspath("./")

    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence("topology.xml")

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = xml_modules.get_xml_info_new(target_folder_xml)

    # shadow 실행
    print("shadow running ...")
    utils.subprocess_open('%s topology.xml > output.txt' % shadowpath)
    output_file = path + "/output.txt" 

    # shadow output 파일 존재 검사.
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest", node_id_list)

    # shadow output 파일 내용 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list, output_file)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # verack 실험 결과 체크 시작.
    check_miniEREBUS(simulation_output_file[0], simulation_output_file[1])


if __name__ == '__main__':
    main(sys.argv[1])
