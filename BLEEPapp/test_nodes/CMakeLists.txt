
## nonblock io test
# add_executable(PEER_NONBLOCK_IO_LOCAL node_nonblock_io.cpp ${SRCS})
add_shadow_plugin(PEER_NONBLOCK_IO node_nonblock_io.cpp)
install(TARGETS PEER_NONBLOCK_IO DESTINATION plugins)
# target_link_libraries(PEER_NONBLOCK_IO_LOCAL ${Boost_LIBRARIES})
target_link_libraries(PEER_NONBLOCK_IO ${Boost_LIBRARIES})


## Asio test
add_shadow_plugin(PEER_ASIO node_asio.cpp)
install(TARGETS PEER_ASIO DESTINATION plugins)
target_link_libraries(PEER_ASIO ${Boost_LIBRARIES})

## libev test
add_shadow_plugin(PEER_LIBEV_SERV node_libev_server.cpp )
install(TARGETS PEER_LIBEV_SERV DESTINATION plugins)
target_link_libraries(PEER_LIBEV_SERV -lev)

add_shadow_plugin(PEER_LIBEV_CLIENT node_libev_client.cpp )
install(TARGETS PEER_LIBEV_CLIENT DESTINATION plugins)
target_link_libraries(PEER_LIBEV_CLIENT -lev)

## pipe test
add_shadow_plugin(PEER_PIPE node_test_pipe.cpp)
install(TARGETS PEER_PIPE DESTINATION plugins)
target_link_libraries(PEER_PIPE -lpthread -lev BLEEP::SHADOW_INTERFACE)
add_executable(PEER_PIPE_LOCAL node_test_pipe.cpp)
target_link_libraries(PEER_PIPE_LOCAL -lpthread -lev BLEEP::SHADOW_INTERFACE)
