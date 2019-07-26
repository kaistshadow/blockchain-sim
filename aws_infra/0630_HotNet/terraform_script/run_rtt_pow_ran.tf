#################################################
# execute bleep experiments
#################################################

resource "null_resource" "bleep_seoul0" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_seoul.*.public_ip, 0)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT_POW_RAN -id=${element(aws_instance.bleep_seoul.*.public_ip, 0)} -fanout=3 -miningtime=10 -miningtimedev=0.5 -connect=${element(aws_instance.bleep_seoul.*.public_ip, 0)} -connect=${element(aws_instance.bleep_virginia.*.public_ip, 0)} -connect=${element(aws_instance.bleep_canada.*.public_ip, 0)} -connect=${element(aws_instance.bleep_california.*.public_ip, 0)} -connect=${element(aws_instance.bleep_ireland.*.public_ip, 0)} -connect=${element(aws_instance.bleep_paulo.*.public_ip, 0)}> bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep_virginia0" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_virginia.*.public_ip, 0)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT_POW_RAN -id=${element(aws_instance.bleep_virginia.*.public_ip, 0)} -fanout=3 -miningtime=10 -miningtimedev=0.5 -connect=${element(aws_instance.bleep_seoul.*.public_ip, 0)} -connect=${element(aws_instance.bleep_virginia.*.public_ip, 0)} -connect=${element(aws_instance.bleep_canada.*.public_ip, 0)} -connect=${element(aws_instance.bleep_california.*.public_ip, 0)} -connect=${element(aws_instance.bleep_ireland.*.public_ip, 0)} -connect=${element(aws_instance.bleep_paulo.*.public_ip, 0)}> bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep_canada0" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_canada.*.public_ip, 0)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT_POW_RAN -id=${element(aws_instance.bleep_canada.*.public_ip, 0)} -fanout=3 -miningtime=10 -miningtimedev=0.5 -connect=${element(aws_instance.bleep_seoul.*.public_ip, 0)} -connect=${element(aws_instance.bleep_virginia.*.public_ip, 0)} -connect=${element(aws_instance.bleep_canada.*.public_ip, 0)} -connect=${element(aws_instance.bleep_california.*.public_ip, 0)} -connect=${element(aws_instance.bleep_ireland.*.public_ip, 0)} -connect=${element(aws_instance.bleep_paulo.*.public_ip, 0)}> bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep_california0" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_california.*.public_ip, 0)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT_POW_RAN -id=${element(aws_instance.bleep_california.*.public_ip, 0)} -fanout=3 -miningtime=10 -miningtimedev=0.5 -connect=${element(aws_instance.bleep_seoul.*.public_ip, 0)} -connect=${element(aws_instance.bleep_virginia.*.public_ip, 0)} -connect=${element(aws_instance.bleep_canada.*.public_ip, 0)} -connect=${element(aws_instance.bleep_california.*.public_ip, 0)} -connect=${element(aws_instance.bleep_ireland.*.public_ip, 0)} -connect=${element(aws_instance.bleep_paulo.*.public_ip, 0)}> bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep_ireland0" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_ireland.*.public_ip, 0)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT_POW_RAN -id=${element(aws_instance.bleep_ireland.*.public_ip, 0)} -fanout=3 -miningtime=10 -miningtimedev=0.5 -connect=${element(aws_instance.bleep_seoul.*.public_ip, 0)} -connect=${element(aws_instance.bleep_virginia.*.public_ip, 0)} -connect=${element(aws_instance.bleep_canada.*.public_ip, 0)} -connect=${element(aws_instance.bleep_california.*.public_ip, 0)} -connect=${element(aws_instance.bleep_ireland.*.public_ip, 0)} -connect=${element(aws_instance.bleep_paulo.*.public_ip, 0)}> bleep.out &';
    EOT
  }
}

resource "null_resource" "bleep_paulo0" {
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_paulo.*.public_ip, 0)} 'cd blockchain-sim/BLEEPapp/build; stdbuf -o0 ./LOCAL_NODE_RTT_POW_RAN -id=${element(aws_instance.bleep_paulo.*.public_ip, 0)} -fanout=3 -miningtime=10 -miningtimedev=0.5 -connect=${element(aws_instance.bleep_seoul.*.public_ip, 0)} -connect=${element(aws_instance.bleep_virginia.*.public_ip, 0)} -connect=${element(aws_instance.bleep_canada.*.public_ip, 0)} -connect=${element(aws_instance.bleep_california.*.public_ip, 0)} -connect=${element(aws_instance.bleep_ireland.*.public_ip, 0)} -connect=${element(aws_instance.bleep_paulo.*.public_ip, 0)}> bleep.out &';
    EOT
  }
}

