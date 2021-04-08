import sys
import os

sys.path.append('../../../../')
from testlibs import test_modules


def check_shadownode(monero_log, tester_log):
    f = open(monero_log, "r")
    iterf = iter(f)
    check_flag = [False, False, False, False, False, False]
    for line in iterf:
        # check whether the connection is established
        if "NEW CONNECTION" in line:
            if "INC" in line:
                check_flag[0] = True
                print("CHECK : incoming connection established")

        # check whether the COMMAND_HANDSHAKE message is received
        if check_flag[0] and not check_flag[1] and "243 bytes sent for category command-1001 initiated by peer" in line:
            check_flag[1] = True
            print("CHECK : receive COMMAND_HANDSHAKE msg from peer")

        # check whether the first COMMAND_TIMED_SYNC message is sent and received a valid response
        if check_flag[1] and not check_flag[2] and "172 bytes sent for category command-1002 initiated by us" in line:
            check_flag[2] = True
            print("CHECK : first COMMAND_TIMED_SYNC req is sent")

        if check_flag[2] and not check_flag[3] and "bytes received for category command-1002 initiated by us" in line:
            check_flag[3] = True
            print("CHECK : first COMMAND_TIMED_SYNC response is received")

        # check whether the new CONNECTION is established
        if check_flag[3] and not check_flag[4] and "OUT] NEW CONNECTION" in line:
            check_flag[4] = True
            print("CHECK : new outgoing connection is established")

        # check whether the COMMAND_HANDSHAKE is successfully finished
        if check_flag[4] and not check_flag[5] and "received for category command-1001 initiated by us" in line:
            if "OUT" in line:
                check_flag[5] = True
                print("CHECK : COMMAND_HANDSHAKE is successfully responsed")


    if all(check_flag):
        print("Success checking output results of the experiment(shadownode) ...")
        return
    else:
        print("Fail checking output results of the experiment(shadownode) ...")
        exit(-1)


def check_verack_withip(monero_log, tester_log, ip):
    f = open(monero_log, "r")
    iterf = iter(f)
    check_flag = [False, False, False, False, False]
    for line in iterf:
        # check whether the connection is established
        if "NEW CONNECTION" in line:
            if "OUT" in line and ip in line:
                check_flag[0] = True
                print("CHECK : outgoing connection established")

        # check whether the first COMMAND_TIMED_SYNC message is sent and received a valid response
        if check_flag[0] and not check_flag[1] and "bytes sent for category command-1002 initiated by us" in line:
            if ip in line:
                check_flag[1] = True
                print("CHECK : first COMMAND_TIMED_SYNC req is sent for %s" % ip)

        if check_flag[1] and not check_flag[2] and "bytes received for category command-1002 initiated by us" in line:
            if ip in line:
                check_flag[2] = True
                print("CHECK : first COMMAND_TIMED_SYNC response is received for %s" % ip)

        # check whether the second COMMAND_TIMED_SYNC (ping) message is sent and received
        if check_flag[2] and not check_flag[3] and "172 bytes sent for category command-1002 initiated by us" in line:
            if ip in line:
                check_flag[3] = True
                print("CHECK : second COMMAND_TIMED_SYNC req is sent for %s" % ip)

        if check_flag[3] and not check_flag[4] and "bytes received for category command-1002 initiated by us" in line:
            if ip in line:
                check_flag[4] = True
                print("CHECK : second COMMAND_TIMED_SYNC response is received for %s" % ip)


    if all(check_flag):
        print("Success checking output results of the experiment(shadownode-verack) ...")
        return
    else:
        print("Fail checking output results of the experiment(shadownode-verack) ...")
        exit(-1)




def main(shadowpath, configpath):
    # xml 파일 생성 확인
    target_folder_xml = test_modules.test_xml_existence(configpath)

    # 생성된 xml 파일로 부터 runtime, node_id, plugin 들을 뽑아옴.
    runtime, node_id_list, plugin_list = test_modules.get_xml_info_new(target_folder_xml)

    # shadow 실행
    print("shadow running ...")
    test_modules.subprocess_open('%s --turn_off_tls_fix 0 %s > output.txt' % (shadowpath, configpath))

    # shadow output 파일 존재 검사.
    target_folder_file = test_modules.test_shadow_output_file_existence("regtest")

    # shadow output 파일 내용 검사.
    test_modules.test_shadow(target_folder_file, runtime, node_id_list)

    # shadow plugin의 결과 값 뽑아옴.
    simulation_output_file = test_modules.test_file_existence(node_id_list, plugin_list)

    # shadownode 실험 결과 체크 시작.
    # check_shadownode(simulation_output_file[0], simulation_output_file[1])

    check_verack_withip(simulation_output_file[0], simulation_output_file[1], "3.0.0.1")


if __name__ == '__main__':
    main(sys.argv[1], sys.argv[2])
