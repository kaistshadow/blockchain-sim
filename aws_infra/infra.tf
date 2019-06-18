variable "image_id" {
  type = "string"
  default = "ami-05c33d61792a62bec"
}

resource "aws_key_pair" "admin" {
  key_name = "admin"
  public_key = "${file("admin.pub")}"
}        

resource "aws_security_group" "ssh" {
  name = "allow_ssh_from_all"
  description = "Allow SSH port from all"
  ingress {
    from_port = 22
    to_port = 22
    protocol = "tcp"
    cidr_blocks = ["0.0.0.0/0"]
  }
}

data "aws_security_group" "default" {
  name = "default"
}

#################################################
# deploy bleep nodes for amazon ec2
#################################################

resource "aws_instance" "bleep" {
  count = 10
  ami = "${var.image_id}"
  instance_type = "t2.medium"
  key_name = "${aws_key_pair.admin.key_name}"
  vpc_security_group_ids = [
    "${aws_security_group.ssh.id}",
    "${data.aws_security_group.default.id}"
  ]
}

resource "null_resource" "print_ip" {
  count = 10
  provisioner "local-exec" {
    command = <<EOT
    echo "amazonip=${element(aws_instance.bleep.*.public_ip, count.index)}"
    echo "amazonprivip=${element(aws_instance.bleep.*.private_ip, count.index)}"
    EOT
  }
}
