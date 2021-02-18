import os
from subprocess import check_output
import argparse
import sys

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def filter_target_logs(args_list):
    args_filter = []
    args_xml_application = ""
    for i in range(1,len(args_list)):
        args_xml_application += " -"
        args_xml_application += args_list[i]
        result = args_list[i].find("datadir")
        if result != -1:
            # datadir means the "datadir" flag is activated
            args_filter.append("datadir")
        result = args_list[i].find("port")
        if result != -1:
            result = args_list[i].find("rpc")
            if result != -1:
                # "Bound to port 11111" text means the rpcport flag is activated
                args_filter.append("Bound to port 11111")
            else:
                # "AddLocal" text means the port flag is activated 
                args_filter.append("AddLocal")
    return args_xml_application, args_filter

def main():

    # difficulty setting
    # mining algorithm option setting
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("--latency", metavar="latency", default="50.0", help="network latency")
    parser.add_argument("--packetloss", metavar="packetloss", default="0.0", help="network packetloss")
    parser.add_argument("--bandwidthup", metavar="bandwidthup", default="10000240", help="bandwidthup latency")
    parser.add_argument("--bandwidthdown", metavar="bandwidthdown", default="10000020000040", help="bandwidthdown latency")
    parser.add_argument("--runtime", metavar="runtime", default="10", help="simulation runtime")
    parser.add_argument("--node", metavar="nodeid", help="blockchain plugin node id")
    parser.add_argument("--args", metavar="applicationArgs", help="blockchain application args")
    args = parser.parse_args()

    args_confirm = args.args
    args_list = args_confirm.split("-")
    ###############################
    ## make args standard (bitcoin application args)
    ###############################
    args_xml_application, args_filter = filter_target_logs(args_list) 
    
           
    ################################
    ## make xml file (shadow xml file)
    ################################
    args_xml = "--runtime" + args.runtime + " --latency=" +  args.latency + " --packetloss=" + args.packetloss + " --bandwidthup=" + args.bandwidthup + " --bandwidthdown=" + args.bandwidthdown + " --node=" + args.node
    xml_command = "python xmlGenerator.py " + args_xml + args_xml_application
    # exec_shell_cmd(xml_command)

    ################################
    ## run shadow with xml made above (shadow xml file and bitcoin filtered args)
    ################################
    exec_shell_cmd("shadow output.xml")
    path = os.path.abspath(".")
    target_folder = path + "/shadow.data/hosts/"
    target_folder_sub = args.node + "/stdout-" + args.node + ".bitcoind.1000.log"
    target_folder += target_folder_sub
    count = 0

    ################################
    ## compare shadow output with args standard
    ################################
    if os.path.isfile(target_folder):
        f = open(target_folder, "r")
        while True:
            line = f.readline()
            if not line: break
            result = line.find("UpdateTip: new best")
            if result != -1:
                result = line.find("height=0")
                if result != -1:
                    sys.exit(0)
                else:
                    sys.exit(1)

        f.close()
    else:
        sys.exit(1)
if __name__ == '__main__':
    main()