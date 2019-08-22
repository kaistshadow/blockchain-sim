import os
from subprocess import check_output, Popen, PIPE, STDOUT, CalledProcessError
import argparse
import sys
import time

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


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Script for installation and simulation on amazon')
    parser.add_argument("--install", action="store_true", help="Install images")

    args = parser.parse_args()
    OPT_INSTALL = args.install

    ipaddrs = []
    privipaddrs = []
    
    if OPT_INSTALL:
        # newly running terraform 
        with open("cur_ips", "w") as f:
            for line in exec_shell_cmd("terraform apply -auto-approve").splitlines():
                print line
                if "amazonip" in line and not "Executing" in line:
                    ip = line.split("=")[1]
                    ipaddrs.append(ip)
                    f.write("pub:"+ip+"\n")
                if "amazonprivip" in line and not "Executing" in line:
                    ip = line.split("=")[1]
                    privipaddrs.append(ip)
                    f.write("priv:"+ip+"\n")
            # # add key
            # for ip in ipaddrs:
            #     time.sleep(0.1)
            #     with open(os.path.expanduser("~/.ssh/known_hosts"), "a") as f:
            #         p = Popen(["ssh-keyscan", ip], stdout=f)
            #         p.communicate() # wait for termination
    else:
        # ip from file
        with open("cur_ips", "r") as f:
            for line in f:
                if "pub" in line:
                    ipaddrs.append(line.split(":")[1].strip())
                elif "priv" in line:
                    privipaddrs.append(line.split(":")[1].strip())

    if ipaddrs == []:
        print "no valid ip address"
        exit(-1)

    print ipaddrs
    print privipaddrs


    # # install ntp
    # for idx, ip in enumerate(ipaddrs):
    #     if idx == 0: # ntp server
    #         print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'sudo apt-get install ntp -y; sudo cp /etc/ntp.conf /etc/ntp.conf.backup; sudo sh -c "echo \'server 127.127.1.0 iburst minpoll 4\' > /etc/ntp.conf"; sudo sh -c "echo \'fudge 127.127.1.0 stratum 10\' >> /etc/ntp.conf"; sudo sh -c "echo \'restrict 127.0.0.1\' >> /etc/ntp.conf"; sudo sh -c "echo \'restrict ::1\' >> /etc/ntp.conf"; ']).strip()
    #         print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'sudo ufw allow from any to any port 123 proto udp']).strip() # firewall setting for ntp https://vitux.com/how-to-install-ntp-server-and-client-on-ubuntu/
        
    #     if not idx == 0:
    #         print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'sudo apt-get install ntp -y; sudo cp /etc/ntp.conf /etc/ntp.conf.backup; sudo sh -c "echo \'server 127.127.1.0\' > /etc/ntp.conf"; sudo sh -c "echo \'fudge 127.127.1.0 stratum 10\' >> /etc/ntp.conf"; sudo sh -c "echo \'restrict 127.0.0.1\' >> /etc/ntp.conf"; sudo sh -c "echo \'restrict ::1\' >> /etc/ntp.conf"; sudo sh -c "echo \'server %s iburst minpoll 4\' >> /etc/ntp.conf"; ' % privipaddrs[0] ]).strip()
    #         print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'sudo timedatectl set-ntp off']).strip() # disable systemd timesyncd for ntp https://vitux.com/how-to-install-ntp-server-and-client-on-ubuntu/

    # # execute ntp
    # for ip in ipaddrs:
    #     print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'sudo /etc/init.d/ntp restart'])
        

    # # check ntp
    # for ip in ipaddrs:
    #     print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'ntpq -p'])


    
    # # prepare git repository  (parallel version)
    # exec_shell_cmd("mv prepare_repo_multiregion.tf_ prepare_repo_multiregion.tf")
    # print exec_shell_cmd("terraform apply -auto-approve")

    # # # prepare git repository
    # for ip in ipaddrs:
    #     print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'cd blockchain-sim/; ssh-keyscan -p 2222 143.248.38.37 >> ~/.ssh/known_hosts; git remote add localstream ssh://ilios@143.248.38.37:2222/home/ilios/hdd/git-server/bleep.git; ssh-agent bash -c "ssh-add /home/ubuntu/bleep_admin; git pull localstream yg-exp"; git checkout yg-exp'])            

    # # # prepare git repository
    # for ip in ipaddrs:
    #     print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'cd blockchain-sim/; ssh-agent bash -c "ssh-add /home/ubuntu/bleep_admin; git pull localstream yg-exp"; git checkout yg-exp'])            
    


    
    # # checkout git
    # for ip in ipaddrs:
    #     print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'cd blockchain-sim/; git checkout yg-exp'])    
    # # git checkout yg-exp; git checkout 8efb472'


    # # check git repository 
    # githead_set = set()
    # for ip in ipaddrs:
    #     githead = exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'cd blockchain-sim/; git rev-parse HEAD']).splitlines()[1].strip()
    #     githead_set.add(githead)
    #     if len(githead_set) != 1:
    #         print "for %s, wrong git head:%s" % (ip, githead)
    #         exit(-1)
    #     else:
    #         print "for %s, git head:%s" % (ip, githead)

    # reset prepare BLEEP install, launch experiment
    # reset launch experiment
    # exec_shell_cmd("mv prepare_install_multiregion.tf prepare_install_multiregion.tf_")
    # exec_shell_cmd("mv run_pow_random.tf run_pow_random.tf_")
    # exec_shell_cmd("mv run_pow_random_notxgen.tf run_pow_random_notxgen.tf_")
    # exec_shell_cmd("mv run_pow_random_notxgen_isol.tf run_pow_random_notxgen_isol.tf_")
    # exec_shell_cmd("mv run_pow_random_ctxtest.tf run_pow_random_ctxtest.tf_")

    # exec_shell_cmd("mv run_rtt_6node.tf run_rtt_6node.tf_")
    # print exec_shell_cmd("terraform apply -auto-approve")

    # exec_shell_cmd("mv run_pow_random_rtt.tf run_pow_random_rtt.tf_")
    # print exec_shell_cmd("terraform apply -auto-approve")




    # # prepare BLEEP install
    # exec_shell_cmd("mv prepare_install_multiregion.tf prepare_install_multiregion.tf_")
    # print exec_shell_cmd("terraform apply -auto-approve")

    # # prepare BLEEP install
    # exec_shell_cmd("mv prepare_install_multiregion.tf_ prepare_install_multiregion.tf")
    # print exec_shell_cmd("terraform apply -auto-approve")

    # # # prepare BLEEP (install) serial
    # for ip in ipaddrs:
    #     print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'cd blockchain-sim/; mkdir -p BLEEPlib/build; cd BLEEPlib/build; cmake ../; make install; cd ../../BLEEPapp/build; cmake ../; make install'])            
    

    # # launch experiment
    # exec_shell_cmd("mv run_rtt.tf_ run_rtt.tf")
    # print exec_shell_cmd("terraform apply -auto-approve")

    # # launch experiment
    # exec_shell_cmd("mv run_rtt_6node.tf_ run_rtt_6node.tf")
    # print exec_shell_cmd("terraform apply -auto-approve")

    # # launch experiment
    # exec_shell_cmd("mv run_rtt_pow_ran.tf_ run_rtt_pow_ran.tf")
    # print exec_shell_cmd("terraform apply -auto-approve")




    # # get results
    # for idx, ip in enumerate(ipaddrs):
    #     exec_shell_cmd_list(["scp", "-r", "-i", "admin", "ubuntu@%s:~/blockchain-sim/BLEEPapp/build/bleep.out" % ip, "ec2_outputs/stdout-node%d.out-%s" % (idx, ipaddrs[idx])])

    # # get status
    # for idx, ip in enumerate(ipaddrs):
    #     print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, 'ps aux | grep NODE'])

    # # terminate
    # for idx, ip in enumerate(ipaddrs):
    #     print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, "ps x | grep NODE | awk '{print $1}' | head -n 1 | xargs kill"])
    
    # # get context switch
    # for idx, ip in enumerate(ipaddrs):
    #     print exec_shell_cmd_list(["ssh", "-i", "admin", "ubuntu@%s" % ip, "pgrep NODE | xargs -I{} grep ctx /proc/{}/status"])

    for i in range(36,41):
        print i
        exec_shell_cmd_system("rm -f ec2_outputs/*")
        time.sleep(5)
        
        if os.path.exists("run_rtt_pow_ran.tf"):
            # destroy experiment
            exec_shell_cmd("mv run_rtt_pow_ran.tf run_rtt_pow_ran.tf_")
            print exec_shell_cmd("terraform apply -auto-approve")
        
        time.sleep(10)
        
        # launch experiment
        exec_shell_cmd("mv run_rtt_pow_ran.tf_ run_rtt_pow_ran.tf")
        print exec_shell_cmd("terraform apply -auto-approve")
        
        time.sleep(1200)
        
        # get results
        for idx, ip in enumerate(ipaddrs):
            exec_shell_cmd_list(["scp", "-r", "-i", "admin", "ubuntu@%s:~/blockchain-sim/BLEEPapp/build/bleep.out" % ip, "ec2_outputs/stdout-node%d.out-%s" % (idx, ipaddrs[idx])])

        # copy results
        exec_shell_cmd("cp -r ec2_outputs 0626_ec2_outputs_rtt_pow_ran_6node_power_%d" % i)
        time.sleep(5)
        
    
    


