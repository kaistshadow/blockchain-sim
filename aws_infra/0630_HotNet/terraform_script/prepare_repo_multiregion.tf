#################################################
# prepare ssh for bleep experiments
#################################################

resource "null_resource" "prepare_repo_seoul" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_seoul.*.public_ip, count.index)} 'cd blockchain-sim/; ssh-keyscan -p 2222 143.248.38.37 >> ~/.ssh/known_hosts; git remote add localstream ssh://ilios@143.248.38.37:2222/home/ilios/hdd/git-server/bleep.git; ssh-agent bash -c "ssh-add /home/ubuntu/bleep_admin; git pull localstream yg-exp";'
    EOT
  }
}

resource "null_resource" "prepare_repo_virginia" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_virginia.*.public_ip, count.index)} 'cd blockchain-sim/; ssh-keyscan -p 2222 143.248.38.37 >> ~/.ssh/known_hosts; git remote add localstream ssh://ilios@143.248.38.37:2222/home/ilios/hdd/git-server/bleep.git; ssh-agent bash -c "ssh-add /home/ubuntu/bleep_admin; git pull localstream yg-exp";'
    EOT
  }
}

resource "null_resource" "prepare_repo_canada" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_canada.*.public_ip, count.index)} 'cd blockchain-sim/; ssh-keyscan -p 2222 143.248.38.37 >> ~/.ssh/known_hosts; git remote add localstream ssh://ilios@143.248.38.37:2222/home/ilios/hdd/git-server/bleep.git; ssh-agent bash -c "ssh-add /home/ubuntu/bleep_admin; git pull localstream yg-exp";'
    EOT
  }
}

resource "null_resource" "prepare_repo_california" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_california.*.public_ip, count.index)} 'cd blockchain-sim/; ssh-keyscan -p 2222 143.248.38.37 >> ~/.ssh/known_hosts; git remote add localstream ssh://ilios@143.248.38.37:2222/home/ilios/hdd/git-server/bleep.git; ssh-agent bash -c "ssh-add /home/ubuntu/bleep_admin; git pull localstream yg-exp";'
    EOT
  }
}

resource "null_resource" "prepare_repo_ireland" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_ireland.*.public_ip, count.index)} 'cd blockchain-sim/; ssh-keyscan -p 2222 143.248.38.37 >> ~/.ssh/known_hosts; git remote add localstream ssh://ilios@143.248.38.37:2222/home/ilios/hdd/git-server/bleep.git; ssh-agent bash -c "ssh-add /home/ubuntu/bleep_admin; git pull localstream yg-exp";'
    EOT
  }
}

resource "null_resource" "prepare_repo_paulo" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_paulo.*.public_ip, count.index)} 'cd blockchain-sim/; ssh-keyscan -p 2222 143.248.38.37 >> ~/.ssh/known_hosts; git remote add localstream ssh://ilios@143.248.38.37:2222/home/ilios/hdd/git-server/bleep.git; ssh-agent bash -c "ssh-add /home/ubuntu/bleep_admin; git pull localstream yg-exp";'
    EOT
  }
}
