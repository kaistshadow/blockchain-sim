#################################################
# prepare bleep experiments
#################################################

resource "null_resource" "bleepinstall_seoul" {
  count = 1                         
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_seoul.*.public_ip, count.index)} 'cd blockchain-sim/; mkdir -p BLEEPlib/build; cd BLEEPlib/build; cmake ../; make install; cd ../../BLEEPapp/build; cmake ../; make install'
    EOT
  }
}

resource "null_resource" "bleepinstall_virginia" {
  count = 1                         
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_virginia.*.public_ip, count.index)} 'cd blockchain-sim/; mkdir -p BLEEPlib/build; cd BLEEPlib/build; cmake ../; make install; cd ../../BLEEPapp/build; cmake ../; make install'
    EOT
  }
}

resource "null_resource" "bleepinstall_canada" {
  count = 1                         
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_canada.*.public_ip, count.index)} 'cd blockchain-sim/; mkdir -p BLEEPlib/build; cd BLEEPlib/build; cmake ../; make install; cd ../../BLEEPapp/build; cmake ../; make install'
    EOT
  }
}

resource "null_resource" "bleepinstall_california" {
  count = 1                         
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_california.*.public_ip, count.index)} 'cd blockchain-sim/; mkdir -p BLEEPlib/build; cd BLEEPlib/build; cmake ../; make install; cd ../../BLEEPapp/build; cmake ../; make install'
    EOT
  }
}

resource "null_resource" "bleepinstall_ireland" {
  count = 1                         
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_ireland.*.public_ip, count.index)} 'cd blockchain-sim/; mkdir -p BLEEPlib/build; cd BLEEPlib/build; cmake ../; make install; cd ../../BLEEPapp/build; cmake ../; make install'
    EOT
  }
}

resource "null_resource" "bleepinstall_paulo" {
  count = 1                         
  provisioner "local-exec" {
    command = <<EOT
    ssh -i admin ubuntu@${element(aws_instance.bleep_paulo.*.public_ip, count.index)} 'cd blockchain-sim/; mkdir -p BLEEPlib/build; cd BLEEPlib/build; cmake ../; make install; cd ../../BLEEPapp/build; cmake ../; make install'
    EOT
  }
}

