#################################################
# prepare bleep experiments
#################################################

resource "null_resource" "bleeppre_" {
  count = 10                         
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep.*.public_ip, count.index)} 'cd blockchain-sim/; mkdir -p BLEEPlib/build; cd BLEEPlib/build; cmake ../; make install; cd ../../BLEEPapp/build; cmake ../; make install'
    EOT
  }
}

