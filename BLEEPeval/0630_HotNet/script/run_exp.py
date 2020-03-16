import os
from subprocess import check_output, Popen, PIPE, STDOUT, CalledProcessError
import argparse
import sys
import time
import datetime

def exec_shell_cmd_system(cmd):
    if os.system(cmd) != 0:
        print("error while executing '%s'" % cmd)
        exit(-1)

def exec_shell_cmd(cmd):
    # if os.system(cmd) != 0:
    #     print("error while executing '%s'" % cmd)
    #     exit(-1)
    print "executing '%s'..." % cmd
    try:
        result = check_output(cmd.split(), stderr=STDOUT)
        return result
        # do something with output
    except CalledProcessError as e:
        print e.output
        print("error while executing '%s'" % cmd)
        exit(-1)
        # There was an error - command exited with non-zero code

def exec_shell_cmd_list(cmds):
    # if os.system(cmd) != 0:
    #     print("error while executing '%s'" % cmd)
    #     exit(-1)
    print "executing '%s'..." % " ".join(cmds)
    try:
        result = check_output(cmds, stderr=STDOUT)
        return result
        # do something with output
    except CalledProcessError as e:
        print e.output
        print("error while executing '%s'" % " ".join(cmds))
        exit(-1)
        # There was an error - command exited with non-zero code

def run_experiment(configfile, datadir, workerthread = 0):
    shadow = Popen([os.path.expanduser("shadow"), "-d", datadir, "-w", str(workerthread), configfile], stdout=PIPE)
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
    



if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for simulation on BLEEP')

    args = parser.parse_args()

    # ## test
    # start = datetime.datetime.now()
    # run_experiment("rc1-rtt-pow-ran-1000node.xml", "0627_datadir_test", 4)
    # end = datetime.datetime.now()
    # elapsed_time = end - start
    # print "elapsed_millisec=%d" % (int(elapsed_time.total_seconds() * 1000))
    # ## test end
    # exit(0)


    for i in range(1,11):
        print i
        new_random_time = (946684800 + 10*i)
        exec_shell_cmd("sed -i s/946684800/%d/g ../shadow/src/main/core/support/shd-definitions.h" % new_random_time)
        time.sleep(5)
        exec_shell_cmd_system("cd ../shadow/build; make install")
        time.sleep(5)
        
        # datadir = "0626_datadir_rtt_pow_ran_6node_%d" % i
        datadir = "0628_datadir_rtt_pow_ran_1000node_%d" % i
        
        start = datetime.datetime.now()
        # run_experiment("config-examples/rc1-rtt-pow-ran-6node-0626lat.xml", datadir)
        # run_experiment("config-examples/rc1-eventloop.xml", datadir, 4)
        run_experiment("rc1-rtt-pow-ran-1000node.xml", datadir, 4)
        end = datetime.datetime.now()
        
        elapsed_time = end - start
        with open("experiment_times_1000node.log", 'a') as f:
            f.write("elapsed_millisec=%d\n" % (int(elapsed_time.total_seconds() * 1000)) )

        exec_shell_cmd("sed -i s/%d/946684800/g ../shadow/src/main/core/support/shd-definitions.h" % new_random_time)
        time.sleep(5)
        exec_shell_cmd_system("rm %s/shadow.output" % datadir)

        # exec_shell_cmd_list(["sed", "-i", "s/946684800/%d/g" % (946684800 + 10*i),  "../shadow/src/main/core/support/shd-definitions.h"] )

