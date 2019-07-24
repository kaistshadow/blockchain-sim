#################################################
# prepare ssh for bleep experiments
#################################################

resource "null_resource" "prepare_repo" {
  count = 6
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, count.index)} 'cd blockchain-sim/; ssh-keyscan -p 2222 143.248.38.37 >> ~/.ssh/known_hosts; git remote add localstream ssh://ilios@143.248.38.37:2222/home/ilios/hdd/git-server/bleep.git; ssh-agent bash -c "ssh-add /home/ubuntu/bleep_admin; git pull localstream yg-exp";'
    EOT
  }
}
