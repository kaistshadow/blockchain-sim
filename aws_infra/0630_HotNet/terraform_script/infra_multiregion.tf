variable "image_id_seoul" {
  type = "string"
  default = "ami-0896c1a9ec062bd90"
}

variable "image_id_virginia" {
  type = "string"
  default = "ami-02d799ee10ad08eb9"
}

variable "image_id_canada" {
  type = "string"
  default = "ami-0651e9c2144921a03"
}

variable "image_id_california" {
  type = "string"
  default = "ami-0eb28eee4c56b713f"
}

variable "image_id_ireland" {
  type = "string"
  default = "ami-0fa3d474587ae9982"
}

variable "image_id_paulo" {
  type = "string"
  default = "ami-0872b8d65892054ac"
}

resource "aws_key_pair" "admin_seoul" {
  provider = "aws.seoul"         
  key_name = "admin"
  public_key = "${file("admin.pub")}"
}        

resource "aws_key_pair" "admin_virginia" {
  provider = "aws.virginia"         
  key_name = "admin"
  public_key = "${file("admin.pub")}"
}        

resource "aws_key_pair" "admin_canada" {
  provider = "aws.canada"         
  key_name = "admin"
  public_key = "${file("admin.pub")}"
}        

resource "aws_key_pair" "admin_california" {
  provider = "aws.california"         
  key_name = "admin"
  public_key = "${file("admin.pub")}"
}        

resource "aws_key_pair" "admin_ireland" {
  provider = "aws.ireland"         
  key_name = "admin"
  public_key = "${file("admin.pub")}"
}        

resource "aws_key_pair" "admin_paulo" {
  provider = "aws.paulo"         
  key_name = "admin"
  public_key = "${file("admin.pub")}"
}        

resource "aws_security_group" "ssh_seoul" {
  provider = "aws.seoul"
  name = "allow_ssh_from_all"
  description = "Allow SSH port from all"
  ingress {
    from_port = 22
    to_port = 22
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

resource "aws_security_group" "ssh_virginia" {
  provider = "aws.virginia"
  name = "allow_ssh_from_all"
  description = "Allow SSH port from all"
  ingress {
    from_port = 22
    to_port = 22
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

resource "aws_security_group" "ssh_canada" {
  provider = "aws.canada"
  name = "allow_ssh_from_all"
  description = "Allow SSH port from all"
  ingress {
    from_port = 22
    to_port = 22
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}
resource "aws_security_group" "ssh_california" {
  provider = "aws.california"
  name = "allow_ssh_from_all"
  description = "Allow SSH port from all"
  ingress {
    from_port = 22
    to_port = 22
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}
resource "aws_security_group" "ssh_ireland" {
  provider = "aws.ireland"
  name = "allow_ssh_from_all"
  description = "Allow SSH port from all"
  ingress {
    from_port = 22
    to_port = 22
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}
resource "aws_security_group" "ssh_paulo" {
  provider = "aws.paulo"
  name = "allow_ssh_from_all"
  description = "Allow SSH port from all"
  ingress {
    from_port = 22
    to_port = 22
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

resource "aws_security_group" "bleepport_seoul" {
  provider = "aws.seoul"
  name = "allow_bleepport_from_all"
  description = "Allow BLEEP port from all"
  ingress {
    from_port = 3456
    to_port = 3456
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

resource "aws_security_group" "bleepport_virginia" {
  provider = "aws.virginia"
  name = "allow_bleepport_from_all"
  description = "Allow BLEEP port from all"
  ingress {
    from_port = 3456
    to_port = 3456
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

resource "aws_security_group" "bleepport_canada" {
  provider = "aws.canada"
  name = "allow_bleepport_from_all"
  description = "Allow BLEEP port from all"
  ingress {
    from_port = 3456
    to_port = 3456
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

resource "aws_security_group" "bleepport_california" {
  provider = "aws.california"
  name = "allow_bleepport_from_all"
  description = "Allow BLEEP port from all"
  ingress {
    from_port = 3456
    to_port = 3456
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

resource "aws_security_group" "bleepport_ireland" {
  provider = "aws.ireland"
  name = "allow_bleepport_from_all"
  description = "Allow BLEEP port from all"
  ingress {
    from_port = 3456
    to_port = 3456
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

resource "aws_security_group" "bleepport_paulo" {
  provider = "aws.paulo"
  name = "allow_bleepport_from_all"
  description = "Allow BLEEP port from all"
  ingress {
    from_port = 3456
    to_port = 3456
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}


data "aws_security_group" "default_seoul" {
  provider = "aws.seoul"
  name = "default"
}

data "aws_security_group" "default_virginia" {
  provider = "aws.virginia"
  name = "default"
}

data "aws_security_group" "default_canada" {
  provider = "aws.canada"
  name = "default"
}
data "aws_security_group" "default_california" {
  provider = "aws.california"
  name = "default"
}
data "aws_security_group" "default_ireland" {
  provider = "aws.ireland"
  name = "default"
}
data "aws_security_group" "default_paulo" {
  provider = "aws.paulo"
  name = "default"
}

#################################################
# deploy bleep nodes for amazon ec2
#################################################

resource "aws_instance" "bleep_seoul" {
  provider = "aws.seoul"
  ami = "${var.image_id_seoul}"
  instance_type = "t2.medium"
  key_name = "${aws_key_pair.admin_seoul.key_name}"
  vpc_security_group_ids = [
    "${aws_security_group.ssh_seoul.id}",
    "${aws_security_group.bleepport_seoul.id}",
    "${data.aws_security_group.default_seoul.id}"
  ]
}

resource "aws_instance" "bleep_virginia" {
  provider = "aws.virginia"
  ami = "${var.image_id_virginia}"
  instance_type = "t2.medium"
  key_name = "${aws_key_pair.admin_virginia.key_name}"
  vpc_security_group_ids = [
    "${aws_security_group.ssh_virginia.id}",
    "${aws_security_group.bleepport_virginia.id}",
    "${data.aws_security_group.default_virginia.id}"
  ]
}

resource "aws_instance" "bleep_canada" {
  provider = "aws.canada"
  ami = "${var.image_id_canada}"
  instance_type = "t2.medium"
  key_name = "${aws_key_pair.admin_canada.key_name}"
  vpc_security_group_ids = [
    "${aws_security_group.ssh_canada.id}",
    "${aws_security_group.bleepport_canada.id}",
    "${data.aws_security_group.default_canada.id}"
  ]
}

resource "aws_instance" "bleep_california" {
  provider = "aws.california"
  ami = "${var.image_id_california}"
  instance_type = "t2.medium"
  key_name = "${aws_key_pair.admin_california.key_name}"
  vpc_security_group_ids = [
    "${aws_security_group.ssh_california.id}",
    "${aws_security_group.bleepport_california.id}",
    "${data.aws_security_group.default_california.id}"
  ]
}
resource "aws_instance" "bleep_ireland" {
  provider = "aws.ireland"
  ami = "${var.image_id_ireland}"
  instance_type = "t2.medium"
  key_name = "${aws_key_pair.admin_ireland.key_name}"
  vpc_security_group_ids = [
    "${aws_security_group.ssh_ireland.id}",
    "${aws_security_group.bleepport_ireland.id}",
    "${data.aws_security_group.default_ireland.id}"
  ]
}

resource "aws_instance" "bleep_paulo" {
  provider = "aws.paulo"
  ami = "${var.image_id_paulo}"
  instance_type = "t2.medium"
  key_name = "${aws_key_pair.admin_paulo.key_name}"
  vpc_security_group_ids = [
    "${aws_security_group.ssh_paulo.id}",
    "${aws_security_group.bleepport_paulo.id}",
    "${data.aws_security_group.default_paulo.id}"
  ]
}

resource "null_resource" "print_ip_seoul" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    echo "amazonip=${element(aws_instance.bleep_seoul.*.public_ip, count.index)}"
    echo "amazonprivip=${element(aws_instance.bleep_seoul.*.private_ip, count.index)}"
    EOT
  }
}

resource "null_resource" "print_ip_virginia" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    echo "amazonip=${element(aws_instance.bleep_virginia.*.public_ip, count.index)}"
    echo "amazonprivip=${element(aws_instance.bleep_virginia.*.private_ip, count.index)}"
    EOT
  }
}

resource "null_resource" "print_ip_canada" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    echo "amazonip=${element(aws_instance.bleep_canada.*.public_ip, count.index)}"
    echo "amazonprivip=${element(aws_instance.bleep_canada.*.private_ip, count.index)}"
    EOT
  }
}

resource "null_resource" "print_ip_california" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    echo "amazonip=${element(aws_instance.bleep_california.*.public_ip, count.index)}"
    echo "amazonprivip=${element(aws_instance.bleep_california.*.private_ip, count.index)}"
    EOT
  }
}

resource "null_resource" "print_ip_ireland" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    echo "amazonip=${element(aws_instance.bleep_ireland.*.public_ip, count.index)}"
    echo "amazonprivip=${element(aws_instance.bleep_ireland.*.private_ip, count.index)}"
    EOT
  }
}

resource "null_resource" "print_ip_paulo" {
  count = 1
  provisioner "local-exec" {
    command = <<EOT
    echo "amazonip=${element(aws_instance.bleep_paulo.*.public_ip, count.index)}"
    echo "amazonprivip=${element(aws_instance.bleep_paulo.*.private_ip, count.index)}"
    EOT
  }
}
