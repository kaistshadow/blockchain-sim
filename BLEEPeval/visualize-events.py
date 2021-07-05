import argparse
import os
import time
import sys
import re
import json
from subprocess import check_output, Popen, PIPE
import datetime

def exec_shell_cmd(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

# Extract eventlog from shadow output file and store it in JSON file 
def parse_output(output, datadir):
    data = {}
    data['eventlogs'] = []
    eventlogs_filename = "eventlogs.json"

    print "Parsing shadow output"
    with open(output) as ifile:
        for line in ifile:
            # Regexp matching is expensive, and eventlog lines are a minority in
            #  shadow.output . So we first use regular pettern matching
            if "shadow_push_eventlog" in line:
                matches = re.findall(r'.*shadow_push_eventlog:(.+?),([0-9]+),(.+?),(.*)', line)
                if len(matches) > 0 and len(matches[0]) >= 3:
                    event = {
                        'host': matches[0][0],
                        'time': matches[0][1],
                        'type': matches[0][2],
                        'args': matches[0][3]
                    }
                    data['eventlogs'].append(event)
    
    print "eventlogs_length=%d" % len(data['eventlogs'])
    with open("./%s/%s" % (datadir, eventlogs_filename), "w+") as ofile:
        json.dump(data, ofile)

    return eventlogs_filename

def run_experiment(configfile, LOGLEVEL, forcerun):
    # Datadir is signed with pid to differenciate experiments currently running on
    # different ports
    datadir = "visualize-datadir.%d" % os.getpid()
    current_config_path = os.path.join(
        os.path.dirname(configfile),
        "current-config.xml"
    )
    exec_shell_cmd("cp %s %s" % (configfile, current_config_path))

    shadow = Popen(["shadow", "-l", LOGLEVEL, "-w", "8", "-d", datadir, configfile], stdout=PIPE)

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
            print
            "critical error occurred during shadow simulation"
            if not forcerun:
                sys.exit(-1)

    if shadow_returnCode != 0:
        print("experiment failed")
        if forcerun:
            pass
        else:
            sys.exit(-1)

    eventlogs_filename = parse_output("./%s/%s" % (datadir, shadow_stdout_filename), datadir)
    
    exec_shell_cmd("rm %s" % current_config_path)
    return "%s/%s/%s" % (os.getcwd(), datadir, eventlogs_filename)

def run_visualization_server(eventlogs, configfile, port, background=False):
    eventlogs_path = os.path.join(os.getcwd(), eventlogs)
    # Relative path from this file (visualize-event.py) to web-gui/
    relpath = "/web-gui"
    wd = os.path.dirname(os.path.realpath(__file__)) + relpath
    if background:
        web_server = Popen(["node", "server.js", eventlogs_path, port], cwd=wd, close_fds=True)
    else:
        web_server = Popen(["node", "server.js", eventlogs_path, port], cwd=wd)
 
    time.sleep(1)
    print ""
    print ""
    print ""
    print ""
    print "The script has launched the NodeJS web server for visualization."
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
    parser.add_argument("--log", help="Shadow Log LEVEL above which to filter messages ('error' < 'critical' < 'warning' < 'message' < 'info' < 'debug') ['message']")
    parser.add_argument("--noserver", action="store_true", help="Don't run visualization server")
    parser.add_argument("--force", action="store_true", help="Run visualization even if the test is failed ['false']")
    # --no server

    args = parser.parse_args()
    configfile = args.configfile
    port = args.port
    OPT_BACKGROUND = args.background
    OPT_NOSERVER = args.noserver
    OPT_FORCE = args.force

    if not args.log:
        LOGLEVEL = "message"
    else:
        LOGLEVEL = args.log

    print "Execute shadow experiment"
    start = datetime.datetime.now()
    output = run_experiment(configfile, LOGLEVEL, OPT_FORCE)
    end = datetime.datetime.now()

    elapsed_time = end - start
    print "elapsed_millisec=%d\n" % (int(elapsed_time.total_seconds() * 1000)) 

    if OPT_NOSERVER:
        exit()
        
    print "Starting visualization"
    if OPT_BACKGROUND:
        run_visualization_server(output, configfile, port, background=True)
    else:
        run_visualization_server(output, configfile, port)

    
