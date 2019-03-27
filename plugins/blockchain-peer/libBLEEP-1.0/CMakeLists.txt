
# debug option
if(BLEEP_DEBUG STREQUAL ON)
  message(STATUS "Debug enabled.")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -std=c++11 -Wall -Werror")
else(BLEEP_DEBUG STREQUAL ON)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")
endif(BLEEP_DEBUG STREQUAL ON)

set(SRCS
  datamodules/Block.cpp
  datamodules/ShadowPipe.cpp
  datamodules/Pipe.cpp
  datamodules/Socket.cpp
  datamodules/Message.cpp
  datamanagermodules/TxPool.cpp
  datamanagermodules/ShadowPipeManager.cpp
  datamanagermodules/PipeManager.cpp
  datamanagermodules/SocketManager.cpp
  datamanagermodules/PeerManager.cpp
  utility/UInt256.cpp
  utility/Assert.cpp
)


add_library(BLEEP-1.0 SHARED ${SRCS})
install(TARGETS BLEEP-1.0 DESTINATION lib)
target_link_libraries(BLEEP-1.0 SHADOW_INTERFACE)
target_link_libraries(BLEEP-1.0 ${Boost_LIBRARIES})