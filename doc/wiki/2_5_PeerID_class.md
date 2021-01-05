_PeerId_ class는 네트워크를 구성하는 peer 들의 identification 을 위해 사용하는 class이다. 
각각의 peer는 unique한 id를 가져야만 한다. 이를 위해 ip와 같은 unique한 string을 인자로 받아 _PeerId_를 생성하게 된다. 
결과적으로 모든 노드는 네트워크 통신을 하기 위해 자신만의 _PeerId_ 를 가지게 된다. 
또한, 네트워크 통신 단위인 _Message_ class에는 message를 보내는 사람과 받는 사람의 _PeerId_ 가 포함되어야만 한다. 
_PeerId_ class는 서로다른 _PeerId_ object가 같은지 판별하기 위한 비교연산자와 직렬화/역직렬화(boost) 기능을 제공하고 있다.
아래와 같은 API를 제공한다.


### _PeerId_ Constructor API
```c++
PeerId(std::string id) 
```
**id**: peer의 id

string으로 peer의 id를 입력받아 PeerId object를 생성한다. 

### _GetId_ API
```c++
std::string GetId() const
```
**return value**: peer의 id (string)

peer의 string id를 return한다.