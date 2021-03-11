import sys

sys.path.append('../../../../')
from testlibs import test_modules


def check_churnout(bitcoin_log, tester_log):
    f = open(bitcoin_log, "r")
    iterf = iter(f)
    check_flag = [False, False, False, False, False]
    for line in iterf:
        # check whether the connection is tried
        if not check_flag[0] and "trying connection 2.0.0.1" in line:
            check_flag[0] = True
            print("CHECK : connection requested")

        # check whether the connection is established
        if check_flag[0] and "Added connection peer=0" in line:
            check_flag[1] = True
            print("CHECK : connection established")

        # check whether the version message is sent
        if check_flag[1] and "sending version" in line:
            if "peer=0" in line:
                check_flag[2] = True
                print("CHECK : sending version msg")

        # check whether the verack message is received
        if check_flag[2] and "received: verack (0 bytes) peer=0" in line:
            check_flag[3] = True
            print("CHECK : receive verack msg")

        # check whether the connection is disconnected
        if check_flag[3] and "disconnecting peer=0" in line:
            check_flag[4] = True
            print("CHECK : peer disconnected")

    if all(check_flag):
        print("Success checking output results of the experiment(churnout) ...")
        return
    else:
        print("Fail checking output results of the experiment(churnout) ...")
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
    target_folder_file = test_modules.test_shadow_output_file_existence()

    # shadow output 파일 내용 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # churnout 실험 결과 체크 시작.
    check_churnout(simulation_output_file[0], simulation_output_file[1])


if __name__ == '__main__':
    main(sys.argv[1])
