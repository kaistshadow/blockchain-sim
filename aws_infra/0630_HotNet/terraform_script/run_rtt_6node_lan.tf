#################################################
# execute bleep experiments
#################################################

resource "null_resource" "bleep0" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 0)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT -id=${element(aws_instance.bleep.*.private_ip, 0)} -rttstart=5 -rttnum=10 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} > bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep1" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 1)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT -id=${element(aws_instance.bleep.*.private_ip, 1)} -rttstart=25 -rttnum=10 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} > bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep2" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 2)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT -id=${element(aws_instance.bleep.*.private_ip, 2)} -rttstart=45 -rttnum=10 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} > bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep3" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 3)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT -id=${element(aws_instance.bleep.*.private_ip, 3)} -rttstart=65 -rttnum=10 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} > bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep4" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 4)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT -id=${element(aws_instance.bleep.*.private_ip, 4)} -rttstart=85 -rttnum=10 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} > bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep5" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 5)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT -id=${element(aws_instance.bleep.*.private_ip, 5)} -rttstart=105 -rttnum=10 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} > bleep.out &';
    EOT
  }
}

