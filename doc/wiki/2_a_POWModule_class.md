_POWModule_ class 는 간단한 POW 를 구현하기 위해 필요한 API들을 제공하게 된다. 
_AsyncEmulateBlockMining_, _AsyncBlockMining_, _IsMining_, _StopMining_ 라는 API들을 제공하고 있다.

제공하고 있는 API들에 대한 설명은 아래와 같다. 

### _AsyncEmulateBlockMining_ API
```c++
void AsyncEmulateBlockMining(std::shared_ptr<POWBlock> candidateBlk, double avg, double stddev);
```
**candidateBlk**: Mining을 하고자하는 block  
**avg**: Mining emulation을 위해 기다리고자 하는 시간의 평균값  
**stddev**: Mining emulation을 위해 기다리고자 하는 시간의 표준분산값  

_candidateBlk_ 을 넘겨서 valid한 block 을 생성받고자 할 때 사용하는 API 이다. mining을 실제 동작시키지 않고, 타이머를 동작시켜서 mining을 에뮬레이션하게 된다. 타이머가 기다리는 시간은, 인자로 넘기는 _avg_, _stddev_ 값에 따라 정규분포를 따르게 된다.

이 API는 asynchronous한 event를 발생시키는 API 이다. 즉, mining 에뮬레이션을 위한 timer가 끝나면, valid한 block을 생성하고, _EmuBlockMiningComplete_ 라는 event를 발생시키게 된다. 
library 유저는 _MainEventManager_ 의 event queue에 접근하여, valid한 block에 대한 포인터를 얻을 수 있다.

### _AsyncBlockMining_ API
```c++
void AsyncBlockMining(std::shared_ptr<POWBlock> candidateBlk, UINT256_t difficulty);
```
**candidateBlk**: Mining을 하고자하는 block
**difficulty**: Mining의 난이도

_candidateBlk_ 을 넘겨서 valid한 block 을 생성받고자 할 때 사용하는 API 이다. _candidateBlk_ 에 nonce 및 timestamp 값을 추가시켜서 실제 해시값을 계산하는 mining 을 수행한다. mining을 수행하는 것은 많은 계산량이 필요한 작업이므로, 별도의 thread를 하나 생성하여 mining을 수행하도록 되어 있다. 즉, API 자체는 곧바로 리턴하지만, 내부적으로 mining을 위한 별도의 thread가 생성되어 해시계산 등의 작업을 수행하게 된다. 이렇게 생성된 thread는 마이닝을 완료되면(즉, valid한 블록을 생성하면) 자동적으로 종료되며, 생성된 블록에 대한 정보를 asynchronous event(BlockMiningComplete)에 담아 전달하게 된다. 


 _difficulty_ 는 threshold값을 의미하는 256bit integer이며, 예를 들어 만일 0x0000....00ffff라는 값을 _difficulty_로 받았다면, 블록의 해시값이 0x0000...00abab와 같이 _difficulty_값보다 작을 경우에 valid한 block으로 판별하게 된다. 즉, mining과정에서 계산한 해시값이 인자로 입력받은 _difficulty_ 보다 낮으면, valid한 block으로 판단한다. 

 _AsyncBlockMining_ 의 동작 시간을 shadow 시뮬레이터에서 올바르게 모델링하기 위해, 우리는 mining thread가 `usleep`(실제로는 `shadow_usleep`) 를 사용하도록 구현하였다. 즉, mining thread가 계산을 수행하는 동안 shadow 시뮬레이션의 시간이 전혀 흐르지 않으므로, 이를 흐르게 하기 위해 인위적으로 sleep을 수행하도록 한 것이다. 현재 mining thread는 1000번동안 random한 nonce 값을 생성하여 해시값을 계산하고, `usleep`을 호출한다. 이때 sleep을 하는 시간은 i7 3.4GHz 머신에서 실제로 1000번동안 해시값을 계산하는데 걸렸던 평균 시간을 측정하여 사용하였다(5850us). 또한, 노드가 shadow 시뮬레이터에서 동작하는 것이 아니고, 실제 머신에서 동작할 경우에는 `usleep`을 동작시킬 필요는 없으므로, 이를 위해 `shadow_usleep` 를 구현하였으며 `usleep`대신에 사용하도록 했다. `shadow_usleep` 함수는, 노드가 shadow시뮬레이터 위에서 동작할때에만 `usleep`을 호출하게 되고, 만일 노드가 shadow 시뮬레이터상에서 동작하지 않을 때는 아무 작업도 하지 않는다. 

 하지만, 위와 같이 구현했음에도 불구하고, 현재 _AsyncBlockMining_ 의 동작 시간은 shadow 시뮬레이터에서 제대로 기록되지 않는 버그가 존재하는 것으로 확인되었다. _AsyncBlockMining_ API 가 내부적으로 생성하는 mining thread 의 shadow 시뮬레이션 시간과, API를 호출하는 main thread의 shadow 시뮬레이션 시간이 서로 싱크가 맞지 않는 현상이 존재한다. 즉, mining thread는 `shadow_usleep` 을 호출한 만큼만 시간이 흐르는 반면, main thread의 시간은 이와 상관없이 시간이 흐르고 따라서 mining thread가 종료되었을 때의 시간이 서로 맞지 않는 현상이 보인다. 왜 main thread의 시간이 더 빠르게 시간이 흐르는지 원인은 아직 파악하지 못했다. 이 부분이 shadow 시뮬레이터의 버그인지, BLEEPlib의 버그인지는 추가로 확인할 필요성이 있다. 


### _IsMining_ API
```c++
bool IsMining()
```
**return value** : 현재 mining 중인지를 boolean 값으로 return.

_POWModule_ 이 현재 block을 mining 하는 과정중인지를 return해주는 함수이다. _AsyncEmulateBlockMining_, _AsyncBlockMining_ 중에 어떤 함수를 썼는지에 상관없이 아직 mining을 진행하는 중이라면 true 를 return하게 된다. 즉, mining emulation을 위한 timer가 동작중이거나, 실제 mining을 위한 thread가 동작중일 때, true를 return하게 된다.


### _StopMining_ API
```c++
void StopMining()
```

_POWModule_ 이 현재 block을 mining 이라면 mining 과정을 중지시켜주는 함수이다. _AsyncEmulateBlockMining_, _AsyncBlockMining_ 중에 어떤 함수를 썼는지에 상관없이 mining을 멈추게 된다. 

