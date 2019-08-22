#################################################
# execute bleep experiments
#################################################

resource "null_resource" "bleep0" {
  provisioner "local-exec" {
    command = <<EOT
    sleep 10;
    ssh-keyscan ${element(aws_instance.bleep.*.public_ip, 0)} >> ~/.ssh/known_hosts;
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 0)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_bleep -id=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 0)} 'ls > ls.out';
    ssh-keygen -R ${element(aws_instance.bleep.*.public_ip, 0)};
    EOT
  }
}

resource "null_resource" "bleep1" {
  provisioner "local-exec" {
    command = <<EOT
    sleep 10;
    ssh-keyscan ${element(aws_instance.bleep.*.public_ip, 1)} >> ~/.ssh/known_hosts;
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 1)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_bleep -id=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 0)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 1)} 'ls > ls.out';
    ssh-keygen -R ${element(aws_instance.bleep.*.public_ip, 1)};
    EOT
  }
}

#################################################
# collect the result of the bleep experiments
#################################################

resource "null_resource" "getresult" {
  count = 2        
  provisioner "local-exec" {
    command = <<EOT
    sleep 60;
    ssh-keyscan ${element(aws_instance.bleep.*.public_ip, count.index)} >> ~/.ssh/known_hosts;
    scp -r -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, count.index)}:~/blockchain-sim/BLEEPapp/build/bleep.out ec2_outputs/${count.index}.bleep.out;
    ssh-keygen -R ${element(aws_instance.bleep.*.public_ip, count.index)};
    EOT
  }
}