// 1. 계층화된 API 레이어들을 선언한 헤더파일들을 참조한다.
#include "BL_MainEventManager.h"
#include "BL1_socket/SocketLayer_API.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer_API.h"

// 2. 계층화된 API 레이어 인터페이스를 가르키는 변수 정의
using namespace libBLEEP;
std::unique_ptr<libBLEEP_BL::MainEventManager> libBLEEP_BL::g_mainEventManager;
std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API> libBLEEP_BL::g_SocketLayer_API;
std::unique_ptr<libBLEEP_BL::BL_PeerConnectivityLayer_API> libBLEEP_BL::g_PeerConnectivityLayer_API;

// 3. 사용할 API 레이러 구현체에 대한 헤더파일들을 참조
#include "BL1_socket/SocketLayer.h"
#include "BL2_peer_connectivity/PeerConnectivityLayer.h"

// 4. 유틸리티 함수를 위한 헤더파일 참조
#include "utility/ArgsManager.h"


int main(int argc, char *argv[]) {
    // 커맨드 라인을 파싱하는 유틸리티 함수를 사용
    gArgs.ParseParameters(argc, argv);

    // API 레이어를 생성하여, 위에서 정의한 전역 변수인 API 레이어 인터페이스 변수에 할당한다
    libBLEEP_BL::g_mainEventManager = std::unique_ptr<libBLEEP_BL::MainEventManager>(
            new libBLEEP_BL::MainEventManager(libBLEEP_BL::AsyncEventEnum::AllEvent));
    libBLEEP_BL::g_SocketLayer_API = std::unique_ptr<libBLEEP_BL::BL_SocketLayer_API>(
            new libBLEEP_BL::BL_SocketLayer());
    libBLEEP_BL::g_PeerConnectivityLayer_API = std::unique_ptr<libBLEEP_BL::BL_PeerConnectivityLayer_API>(
            new libBLEEP_BL::BL_PeerConnectivityLayer(gArgs.GetArg("-id", "noid")));

    // 커맨드 라인 아규먼트로 받은 초기 이웃들에 대한 연결을 생성한다.
    for (auto neighborPeerIdStr : gArgs.GetArgs("-connect"))
        libBLEEP_BL::g_PeerConnectivityLayer_API->ConnectPeer(libBLEEP_BL::PeerId(neighborPeerIdStr));


    libBLEEP_BL::g_mainEventManager->Wait();

    return 0;

}






