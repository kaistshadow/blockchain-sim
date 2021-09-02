
BLEEP을 설치하고 실행할때 문제가 발생한다면 아래의 내용을 참고하여 조치하길 바란다. 

- 먼저, 의견 및 버그에 대해 이슈를 작성한다. 
	- **[Questions and Bug Reports](https://github.com/kaistshadow/blockchain-sim/issues/new/choose)**
- 문제 개선을 위해 코드 작성이 필요할 경우, 
	- **브랜치 네이밍 규칙**에 맞게 브랜치를 생성한다. 
		> - **Branch Naming** <br>
		> BLEEP은 develop 브랜치를 이용해 개발을 진행하고 해당 버전 릴리즈가 완료되면 master에 병합하고 있다. 
		>	- release/x.x.x : develop 브랜치로부터 생성
		>	- feature/<issue_number>/<짧은 설명> : 완료 후 develop 브랜치에 병합
		>	- hotfix/<issue_number> : master 브랜치로부터 생성
		>	- issue/<issue_number> : develop, feature, release 브랜치로부터 생성
- 브랜치에서의 작업은 **커밋 규칙**을 참고한다. 
	- **How to write a git commit message** <br>
		>	- 커밋 제목
		>		- 커밋 제목과 본문 사이에 한줄 띄울것
		>		- 커밋 제목은 50자 이내
		>		- 커밋 제목은 첫글자가 대문자로 시작
		>		- 커밋 제목 끝에 온점 붙이지 말 것
		>		- 커밋 제목은 명령형으로 쓸 것
		>			- e.g., Implement A to B, Rename A to B, Fix typo
		>	- 커밋 내용
		>		- 72자 이내로 다음 줄로 개행할 것
		>		- 무엇을 바꿨는지, 왜 바꿨는지 쓸 것

- 작업이 완료되면 **pull request**를 요청한다.






