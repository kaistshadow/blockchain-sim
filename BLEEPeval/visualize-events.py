import argparse
import os
import time
from subprocess import check_output, Popen, PIPE


def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def run_experiment(configfile):
    datadir = "visualize-datadir"
    shadow = Popen([os.path.expanduser("~/.shadow/bin/shadow"), "-d", datadir, configfile], stdout=PIPE)    
    shadow_stdout_filename = "shadow.output"
    shadow_stdout_file = open(shadow_stdout_filename, 'w')

    while shadow.poll() is None:
        l = shadow.stdout.readline()
        shadow_stdout_file.write(l)
    for l in shadow.stdout:
        shadow_stdout_file.write(l)
    shadow_stdout_file.close()

    exec_shell_cmd("mv %s ./%s/%s" % (shadow_stdout_filename, datadir, shadow_stdout_filename))

    shadow_stdout_file = open("./%s/%s" % (datadir, shadow_stdout_filename),'r')
    shadow_returnCode = shadow.returncode
    for line in shadow_stdout_file:
        if "critical" in line:
            print "critical error is occurred during shadow simulation"
            sys.exit(-1)

    if shadow_returnCode != 0:
        print "experiment failed"
        sys.exit(-1)
    
    return "./%s/%s" % (datadir, shadow_stdout_filename)
    

def run_visualization_server(shadowoutput, configfile):
    web_server = Popen(["node", "server.js", "../"+shadowoutput], cwd="./web-gui")

 
    time.sleep(1)
    print ""
    print ""
    print ""
    print ""
    print "The script have launched the NodeJS web server for visualization."
    print "You can see the visualization results of the configuration requested (i.e., %s)" % configfile
    print "To see the results, connect to the server using address 'http://ipaddress_of_this_machine:1337/frontend.html' in web browser."
    print ""
    print ""
    print ""
    print "Type Ctrl-C to terminate the NodeJS web server."
    

    web_server.wait()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("configfile", help="filepath for blockchain network configuration file (shadow xml configuration)")

    args = parser.parse_args()
    configfile = args.configfile

    print "Execute shadow experiment"
    output = run_experiment(configfile)

    run_visualization_server(output, configfile)

    
