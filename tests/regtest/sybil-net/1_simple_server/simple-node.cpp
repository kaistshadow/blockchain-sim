// 1. 계층화된 API 레이어들을 선언한 헤더파일들을 참조한다.
#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"

// 2. 계층화된 API 레이어 인터페이스를 가르키는 변수 정의
using namespace libBLEEP;
using namespace libBLEEP_BL;


// 3. 유틸리티 함수를 위한 헤더파일 참조
#include "utility/ArgsManager.h"


int main(int argc, char *argv[]) {
    // 커맨드 라인을 파싱하는 유틸리티 함수를 사용
    gArgs.ParseParameters(argc, argv);

    // API 레이어들을 초기화한다. 초기화하는 순서가 중요하다.
    MainEventManager::InitInstance(AsyncEventEnum::AllEvent);
    BL_SocketLayer_API::Instance();
    BL_PeerConnectivityLayer_API::InitInstance(gArgs.GetArg("-id", "noid"));

    // 커맨드 라인 아규먼트로 받은 초기 이웃들에 대한 연결을 생성한다.
    for (auto neighborPeerIdStr : gArgs.GetArgs("-connect"))
        BL_PeerConnectivityLayer_API::Instance()->ConnectPeer(PeerId(neighborPeerIdStr));

    // event 관리자를 동작시킴 (Reactor 패턴)
    MainEventManager::Instance()->Wait();

    return 0;
}






