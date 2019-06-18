#################################################
# collect the result of the bleep experiments
#################################################

resource "null_resource" "getresult" {
  count = 10
  provisioner "local-exec" {
    command = <<EOT
    scp -r -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, count.index)}:~/blockchain-sim/BLEEPapp/build/bleep.out ec2_outputs/stdout-node${count.index}.out-${element(aws_instance.bleep.*.private_ip, count.index)};
    ssh-keygen -R ${element(aws_instance.bleep.*.public_ip, count.index)};
    EOT
  }
}