import argparse
import os
import time
import sys
import re
import json
from subprocess import check_output, Popen, PIPE


def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def parse_output(output, datadir):
    start = int(time.time()*1000)
    data = {}
    data['eventlogs'] = []
    eventlogs_filename = "eventlogs.json"

    print "Parsing shadow output"
    with open(output) as ifile:
        for line in ifile:
            if "shadow_push_eventlog" in line:
                matches = re.findall(r'.*shadow_push_eventlog:(.+?),([0-9]+),(.+?),(.*)', line)
                if len(matches) > 0 and len(matches[0]) >= 3:
                    #print line
                    event = {
                        'host': matches[0][0],
                        'time': matches[0][1],
                        'type': matches[0][2],
                        'args': matches[0][3]
                    }
                    data['eventlogs'].append(event)
    
    print "eventlogs length: %d" % len(data['eventlogs'])
    with open("./%s/%s" % (datadir, eventlogs_filename), "w+") as ofile:
        json.dump(data, ofile)

    end = int(time.time()*1000)
    with open("timelogs.txt", "a+") as ifile:
        ifile.write("Converting to json: %s\n" % str((end-start)/1000.0))

    return eventlogs_filename

def run_experiment(configfile):
    start = int(time.time()*1000)
    datadir = "visualize-datadir." + str(os.getpid())
    current_config_path = "%s/current-config.%d.xml" % (os.path.dirname(configfile), os.getpid())
    exec_shell_cmd("cp %s %s" % (configfile, current_config_path))

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

    end = int(time.time()*1000)
    eventlogs_filename = parse_output("./%s/%s" % (datadir, shadow_stdout_filename), datadir)
    other = int(time.time()*1000)

    with open("timelogs.txt", "a+") as ifile:
        ifile.write("\n%s\nExperiment with parsing function: %s\nParsing added time: %s\n" %
            (configfile, str((end-start)/1000.0), str((other-end)/1000.0)))
    
    return "./%s/%s" % (datadir, eventlogs_filename)

def run_experiment_parse(configfile):
    data = {}
    T = 0
    start = int(time.time()*1000)
    data['eventlogs'] = []
    eventlogs_filename = "eventlogs.json"

    datadir = "visualize-datadir." + str(os.getpid())
    current_config_path = "%s/current-config.%d.xml" % (os.path.dirname(configfile), os.getpid())
    exec_shell_cmd("cp %s %s" % (configfile, current_config_path))

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
    
    print "Parsing shadow output"
    for line in shadow_stdout_file:
        if "critical" in line:
            print "critical error is occurred during shadow simulation"
            sys.exit(-1)
        t0 = int(time.time()*1000)
        if "shadow_push_eventlog" in line:
            matches = re.findall(r'.*shadow_push_eventlog:(.+?),([0-9]+),(.+?),(.*)$', line)
            if len(matches) > 0 and len(matches[0]) >= 3:
                event = {
                    'host': matches[0][0],
                    'time': matches[0][1],
                    'type': matches[0][2],
                    'args': matches[0][3]
                }
                data['eventlogs'].append(event)
        T = T + int(time.time()*1000) - t0 
    
    print "eventlogs length: %d" % len(data['eventlogs'])
    with open("%s/%s" % (datadir, eventlogs_filename), "w+") as ofile:
        json.dump(data, ofile)
    
    if shadow_returnCode != 0:
        print "experiment failed"
        sys.exit(-1)
    end = int(time.time()*1000)
    with open("timelogs.txt", "a+") as ifile:
        ifile.write("\n%s\nExperiment with parsing: %s\nParsing added time: %s\n" %
            (configfile, str((end-start)/1000.0), str(T/1000.0)))
    return "./%s/%s" % (datadir, eventlogs_filename)

def run_visualization_server(shadowoutput, configfile, port, background=False):
    if background:
        web_server = Popen(["node", "server.js", "../"+shadowoutput, port], cwd="./web-gui", close_fds=True)
    else:
        web_server = Popen(["node", "server.js", "../"+shadowoutput, port], cwd="./web-gui")

    current_config_path = "%s/current-config.%d.xml" % (os.path.dirname(configfile), os.getpid())
    exec_shell_cmd("rm %s" % current_config_path)
 
    time.sleep(1)
    print ""
    print ""
    print ""
    print ""
    print "The script have launched the NodeJS web server for visualization."
    print "You can see the visualization results of the configuration requested (i.e., %s)" % configfile
    print "To see the results, connect to the server using address 'http://ipaddress_of_this_machine:%s/frontend.html' in web browser." % port
    print ""
    print ""
    print ""
    
    if not background:
        print "Type Ctrl-C to terminate the NodeJS web server."
        web_server.wait()


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation')
    parser.add_argument("configfile", help="filepath for blockchain network configuration file (shadow xml configuration)")
    parser.add_argument("-p", "--port", metavar="port", default="1337", help="Port where we'll run the websocket server")
    parser.add_argument("--background", action="store_true", help="Run server as background daemon.")

    args = parser.parse_args()
    configfile = args.configfile
    port = args.port
    OPT_BACKGROUND = args.background

    print "Execute shadow experiment"
    output = run_experiment(configfile)

    print "Starting visualization"
    if OPT_BACKGROUND:
        run_visualization_server(output, configfile, port, background=True)
    else:
        run_visualization_server(output, configfile, port)

    
