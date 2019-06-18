#################################################
# execute bleep experiments
#################################################

resource "null_resource" "bleep0" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 0)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 0)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 0)} 'ls > ls.out';
    EOT
  }
}

resource "null_resource" "bleep1" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 1)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 1)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 1)} 'ls > ls.out';
    EOT
  }
}

resource "null_resource" "bleep2" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 2)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 2)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 1)} 'ls > ls.out';
    EOT
  }
}

resource "null_resource" "bleep3" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 3)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 3)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 3)} 'ls > ls.out';
    EOT
  }
}

resource "null_resource" "bleep4" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 4)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 4)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 4)} 'ls > ls.out';
    EOT
  }
}

resource "null_resource" "bleep5" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 5)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 5)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 5)} 'ls > ls.out';
    EOT
  }
}

resource "null_resource" "bleep6" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 6)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 6)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 6)} 'ls > ls.out';
    EOT
  }
}

resource "null_resource" "bleep7" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 7)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 7)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 7)} 'ls > ls.out';
    EOT
  }
}

resource "null_resource" "bleep8" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 8)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 8)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 8)} 'ls > ls.out';
    EOT
  }
}

resource "null_resource" "bleep9" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 9)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_POW_RANDOM -id=${element(aws_instance.bleep.*.private_ip, 9)} -fanout=10 -miningtime=5 -miningtimedev=0.5 -txgeninterval=0.5 -connect=${element(aws_instance.bleep.*.private_ip, 0)} -connect=${element(aws_instance.bleep.*.private_ip, 1)} -connect=${element(aws_instance.bleep.*.private_ip, 2)} -connect=${element(aws_instance.bleep.*.private_ip, 3)} -connect=${element(aws_instance.bleep.*.private_ip, 4)} -connect=${element(aws_instance.bleep.*.private_ip, 5)} -connect=${element(aws_instance.bleep.*.private_ip, 6)} -connect=${element(aws_instance.bleep.*.private_ip, 7)} -connect=${element(aws_instance.bleep.*.private_ip, 8)} -connect=${element(aws_instance.bleep.*.private_ip, 9)} > bleep.out &';
    # ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, 9)} 'ls > ls.out';
    EOT
  }
}

