BLEEP 을 통해 개발자는 실제로 동작하는 블록체인 어플리케이션을 개발할 수 있다.

이 튜토리얼에서는 BLEEP으로 개발한 블록체인 노드를 아마존 ec2에서 돌리고 결과를 확인하는 방법을 설명하도록 하겠다.

# Clone BLEEP repository
BLEEP 저장소를 clone하지 않았다면 아래와 같이 다운받도록 한다.
```bash
git clone https://github.com/kaistshadow/blockchain-sim
cd blockchain-sim
```

# Create Amazon EC2 account
Amazon ec2 에서 돌리기 위해서는 당연히 Amazon ec2 계정을 만들어야 하며, 본 튜토리얼에서는 자동화된 툴을 이용해
amazon ec2 를 접근할 것이기 때문에 API를 위한 엑세스키와 시크릿키가 필요하다.  
아래의 링크를 보고, IAM 계정을 생성하고 엑세스키와 시크릿키를 할당받도록 한다.

[Amazon EC2 IAM 계정 생성 방법](https://www.44bits.io/ko/post/publishing_and_managing_aws_user_access_key)

# Executing BLEEP node on Amazon

우선 자신의 개발 환경이 ubuntu linux 환경인지를 확인해야하며,
amazon EC2 설정 자동화를 위한 툴인 terraform과 packer 의 binary 를 다운받도록 한다.

#### Prerequisites:
+ Terraform 설치 ([링크](https://learn.hashicorp.com/terraform/getting-started/install.html))
+ Packer 설치 ([링크](https://www.packer.io/intro/getting-started/install.html))

이제 BLEEP 예제 어플리케이션을 amazon에서 동작시키고 동작 결과를 확인해보도록 하자.
이를 위해 아래와 같이 `aws_infra` 디렉토리로 이동하자.
```bash
cd ./aws_infra
```

가장 먼저, amazon 에서 사용할 machine image(AMI) 파일을 만들어야 한다.   
ubuntu 16.04 LTS 에 BLEEP이 설치된 AMI를 만들도록 하자.  
미리 제작된 packer의 설정파일을 위에서 받은 아마존 엑세스 키, 시크릿 키와 함께 아래와 같이 packer 에 넘겨준다.
```bash
packer build -var 'aws_access_key=<AWS_ACCESS_KEY_ID>' -var 'aws_secret_key=<AWS_SECRET_ACCESS_KEY>' packer.json
```

아래와 같이 자동으로 생성된 AMI의 id가 출력되면 정상적으로 BLEEP이 설치된 AMI가 제작된 것이다. 
```
==> Builds finished. The artifacts of successful builds are:
--> amazon-ebs: AMIs were created:
ap-northeast-2: ami-08fb62e13c9c88528
```

이제 다음으로, 제작한 머신 이미지(AMI)를 amazon ec2의 instance로 배포하고, 동작시켜보도록 한다.
이 과정 역시, 미리 제작한 terraform script를 이용해 쉽게 동작시킬 수 있다.

terraform script는 aws에 키페어, 보안 그룹, 인스턴스 등의 인프라를 세팅하기 위한 `infra.tf` 파일과, 
세팅한 인프라에서 BLEEP node들을 실행시키고 결과를 로컬 머신으로 가져오기 위한 `run_example.tf` 파일로 나뉘어져 있다.
스크립트 파일은 나뉘어져 있지만, terraform 은 두개의 스크립트 파일을 한번에 동작시키게 된다.  
개발자는 자신이 동작시키고 싶은 BLEEP 실험에 맞게 run_example.tf 를 수정할 수 있다.

terraform 스크립트를 동작시키기 전에, 먼저 인프라 세팅을 위해서는 아래와 같이 키페어를 위한 public, private 키들을 생성해야 한다.
```bash
ssh-keygen -t rsa -b 4096 -C "<EMAIL_ADDRESS>" -f "admin" -N ""
```

또, 아웃풋을 복사해 오기 위한 디렉토리를 아래와 같이 생성하자.
```bash
mkdir ec2_outputs
```

마지막으로, 위에서 생성한 엑세스키와 시크릿키, 그리고 AMI의 id를 로컬 환경 변수로 세팅하자.
```bash
export AWS_ACCESS_KEY_ID="<AWS_ACCESS_KEY_ID>"
export AWS_SECRET_ACCESS_KEY="<AWS_SECRET_ACCESS_KEY>"
export AWS_DEFAULT_REGION="ap-northeast-2" 
export TF_VAR_image_id=ami-08fb62e13c9c88528
```

이제 terraform 을 이용해 amazon에서의 실험을 돌릴 수 있다. 아래의 명령어를 사용하자.
```bash
terraform plan
terraform apply
```

실험이 종료된 후에, 아마존 노드에서 두개의 BLEEP 노드가 서로 연결되는 과정을 출력한 결과를 아래와 같이 확인해볼 수 있다.
```bash
less ec2_outputs/0.bleep.out
less ec2_outputs/1.bleep.out
```

아마존 서버 자원을 낭비하지 않기 위해, 실험을 위해 세팅한 노드들을 아래의 명령어로 종료시키도록 한다.
```bash
terraform destroy
```


# References
본 튜토리얼에서 사용한 terraform 스크립트들은 아래의 링크를 기반으로 만들어졌다.
+ https://www.44bits.io/ko/post/terraform_introduction_infrastrucute_as_code
+ https://stackoverflow.com/questions/44679456/how-do-i-access-an-attribute-from-a-counted-resource-within-another-resource