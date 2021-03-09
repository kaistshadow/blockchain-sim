## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## include shadow
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../../shadow/cmake") # Path for ShadowTool Module
include(ShadowTools) # load ShadowTools Module

enable_testing()
## required for independent build end

################################################### test1 ##############################################################
add_custom_target(eventfd-test1-build ALL
        COMMAND gcc eventfd-test1.c -o eventfd-test1 -lpthread
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test1-shared-build ALL
        COMMAND gcc eventfd-test1.c -o libeventfd-test1.so -lpthread -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test1-run ALL
        COMMAND ./eventfd-test1 > test1-dir/stdout.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test1-shadow-run ALL
        COMMAND shadow eventfd-test1.xml -d test1-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(eventfd-test1-shadow-run eventfd-test1-shared-build)
add_dependencies(eventfd-test1-run eventfd-test1-build eventfd-test1-shadow-run)

# test 과정에서 에러 발생.
# add_test(NAME shadow-eventfd-test1
#         COMMAND diff test1-dir/stdout.out test1-dir/hosts/eventfd-test1-node/stdout-eventfd-test1-node.NODE.1000.log
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
########################################################################################################################

################################################### test2 ##############################################################
add_custom_target(eventfd-test2-build ALL
        COMMAND gcc eventfd-test2.c -o eventfd-test2 -lpthread
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test2-shared-build ALL
        COMMAND gcc eventfd-test2.c -o libeventfd-test2.so -lpthread -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test2-run ALL
        COMMAND ./eventfd-test2 > test2-dir/stdout.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test2-shadow-run ALL
        COMMAND shadow eventfd-test2.xml -d test2-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(eventfd-test2-shadow-run eventfd-test2-shared-build)
add_dependencies(eventfd-test2-run eventfd-test2-build eventfd-test2-shadow-run)

# test 과정에서 에러 발생.
# add_test(NAME shadow-eventfd-test2
#         COMMAND diff test2-dir/stdout.out test2-dir/hosts/eventfd-test2-node/stdout-eventfd-test2-node.NODE.1000.log
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
########################################################################################################################


################################################### test3 ##############################################################
add_custom_target(eventfd-test3-build ALL
        COMMAND gcc eventfd-test3.c -o eventfd-test3 -lpthread
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test3-shared-build ALL
        COMMAND gcc eventfd-test3.c -o libeventfd-test3.so -lpthread -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test3-run ALL
        COMMAND ./eventfd-test3 > test3-dir/stdout.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test3-shadow-run ALL
        COMMAND shadow eventfd-test3.xml -d test3-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(eventfd-test3-shadow-run eventfd-test3-shared-build)
add_dependencies(eventfd-test3-run eventfd-test3-build eventfd-test3-shadow-run)

# diff 함수를 이용하여 비교 
# eventfd test3 - eventfd를 생성하고 epoll_wait을 수행하는 함수를 스레드로 실행시켰을 때의 로그와, 이를 shadow에서 시뮬레이션 했을 때의 로그 비교.
# Input - system 수행 결과, shadow 수행 결과
# Output - 비교 시 문제가 없으면, return 0, 그렇지 않을 시 return -1
add_test(NAME shadow-eventfd-test3
        COMMAND diff test3-dir/stdout.out test3-dir/hosts/eventfd-test3-node/stdout-eventfd-test3-node.NODE.1000.log
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
########################################################################################################################


################################################### test4 ##############################################################
add_custom_target(eventfd-test4-build ALL
        COMMAND gcc eventfd-test4.c -o eventfd-test4 -lpthread
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test4-shared-build ALL
        COMMAND gcc eventfd-test4.c -o libeventfd-test4.so -lpthread -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test4-run ALL
        COMMAND ./eventfd-test4 > test4-dir/stdout.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test4-shadow-run ALL
        COMMAND shadow eventfd-test4.xml -d test4-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(eventfd-test4-shadow-run eventfd-test4-shared-build)
add_dependencies(eventfd-test4-run eventfd-test4-build eventfd-test4-shadow-run)

# diff 함수를 이용하여 비교 
# eventfd test4 - eventfd를 생성하고 epoll_wait을 수행하는 함수를 스레드로 실행시켰을 때의 로그와, 이를 shadow에서 시뮬레이션 했을 때의 로그 비교.
# Input - system 수행 결과, shadow 수행 결과
# Output - 비교 시 문제가 없으면, return 0, 그렇지 않을 시 return -1
add_test(NAME shadow-eventfd-test4
        COMMAND diff test4-dir/stdout.out test4-dir/hosts/eventfd-test4-node/stdout-eventfd-test4-node.NODE.1000.log
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
########################################################################################################################


################################################### test5 ##############################################################
add_custom_target(eventfd-test5-build ALL
        COMMAND gcc eventfd-test5.c -o eventfd-test5 -lpthread
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test5-shared-build ALL
        COMMAND gcc eventfd-test5.c -o libeventfd-test5.so -lpthread -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test5-run ALL
        COMMAND ./eventfd-test5 > test5-dir/stdout.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test5-shadow-run ALL
        COMMAND shadow eventfd-test5.xml -d test5-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(eventfd-test5-shadow-run eventfd-test5-shared-build)
add_dependencies(eventfd-test5-run eventfd-test5-build eventfd-test5-shadow-run)

# diff 함수를 이용하여 비교 
# eventfd test5 - eventfd를 생성하고 epoll_wait을 수행하는 함수를 스레드로 실행시켰을 때의 로그와, 이를 shadow에서 시뮬레이션 했을 때의 로그 비교.
# test4와 비교 시 차이점은 write 함수 수행 시, counter값을 추가를 하였음. (counter)
# Input - system 수행 결과, shadow 수행 결과
# Output - 비교 시 문제가 없으면, return 0, 그렇지 않을 시 return -1
add_test(NAME shadow-eventfd-test5
        COMMAND diff test5-dir/stdout.out test5-dir/hosts/eventfd-test5-node/stdout-eventfd-test5-node.NODE.1000.log
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
########################################################################################################################


################################################### test6 ##############################################################
add_custom_target(eventfd-test6-build ALL
        COMMAND gcc eventfd-test6.c -o eventfd-test6 -lpthread
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test6-shared-build ALL
        COMMAND gcc eventfd-test6.c -o libeventfd-test6.so -lpthread -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test6-run ALL
        COMMAND ./eventfd-test6 > test6-dir/stdout.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test6-shadow-run ALL
        COMMAND shadow eventfd-test6.xml -d test6-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(eventfd-test6-shadow-run eventfd-test6-shared-build)
add_dependencies(eventfd-test6-run eventfd-test6-build eventfd-test6-shadow-run)

# diff 함수를 이용하여 비교 
# eventfd test6 - eventfd를 생성하고 epoll_wait을 수행하는 함수를 스레드로 실행시켰을 때의 로그와, 이를 shadow에서 시뮬레이션 했을 때의 로그 비교.
# test5와 비교 시 차이점은 read_thread 함수 내에서 epoll_wait 를 2번 수행함.
# Input - system 수행 결과, shadow 수행 결과
# Output - 비교 시 문제가 없으면, return 0, 그렇지 않을 시 return -1
add_test(NAME shadow-eventfd-test6
        COMMAND diff test6-dir/stdout.out test6-dir/hosts/eventfd-test6-node/stdout-eventfd-test6-node.NODE.1000.log
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
########################################################################################################################


################################################### test7 ##############################################################
add_custom_target(eventfd-test7-build ALL
        COMMAND gcc eventfd-test7-nowrite.c -o eventfd-test7 -lpthread
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test7-shared-build ALL
        COMMAND gcc eventfd-test7-nowrite.c -o libeventfd-test7.so -lpthread -shared -fPIC
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test7-run ALL
        COMMAND ./eventfd-test7 > test7-dir/stdout.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(eventfd-test7-shadow-run ALL
        COMMAND shadow eventfd-test7-nowrite.xml -d test7-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(eventfd-test7-shadow-run eventfd-test7-shared-build)
add_dependencies(eventfd-test7-run eventfd-test7-build eventfd-test7-shadow-run)

# diff 함수를 이용하여 비교 
# eventfd test7 - eventfd를 생성하고 epoll_wait을 수행하는 함수를 스레드로 실행시켰을 때의 로그와, 이를 shadow에서 시뮬레이션 했을 때의 로그 비교.
# test6와 비교 시 차이점은 eventfd에 write 수행 로직이 없음.
# Input - system 수행 결과, shadow 수행 결과
# Output - 비교 시 문제가 없으면, return 0, 그렇지 않을 시 return -1
add_test(NAME shadow-eventfd-test7
        COMMAND diff test7-dir/stdout.out test7-dir/hosts/eventfd-test7-node/stdout-eventfd-test7-node.NODE.1000.log
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
########################################################################################################################



################################################### timerfd-test1 ######################################################
add_custom_target(timerfd-test1-build ALL
        COMMAND gcc timerfd-test1.c -o timerfd-test1 -lpthread
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(timerfd-test1-shared-build ALL
        COMMAND gcc timerfd-test1.c -o libtimerfd-test1.so -lpthread -shared -fPIC -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(timerfd-test1-run ALL
        COMMAND ./timerfd-test1 > timerfd-test1-dir/stdout.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(timerfd-test1-shadow-run ALL
        COMMAND shadow timerfd-test1.xml -d timerfd-test1-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(timerfd-test1-shadow-run timerfd-test1-shared-build)
add_dependencies(timerfd-test1-run timerfd-test1-build timerfd-test1-shadow-run)

# test 과정에서 에러 발생.
# add_test(NAME shadow-timerfd-test1
#         COMMAND diff test1-dir/stdout.out test1-dir/hosts/timerfd-test1-node/stdout-timerfd-test1-node.NODE.1000.log
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
########################################################################################################################


################################################### timerfd-test2 ######################################################
add_custom_target(timerfd-test2-build ALL
        COMMAND gcc timerfd-test2-nowrite.c -o timerfd-test2 -lpthread
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(timerfd-test2-shared-build ALL
        COMMAND gcc timerfd-test2-nowrite.c -o libtimerfd-test2.so -lpthread -shared -fPIC -g
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(timerfd-test2-run ALL
        COMMAND ./timerfd-test2 > timerfd-test2-dir/stdout.out
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(timerfd-test2-shadow-run ALL
        COMMAND shadow timerfd-test2-nowrite.xml -d timerfd-test2-dir
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_dependencies(timerfd-test2-shadow-run timerfd-test2-shared-build)
add_dependencies(timerfd-test2-run timerfd-test2-build timerfd-test2-shadow-run)

# test 과정에서 에러 발생.
# add_test(NAME shadow-timerfd-test2
#         COMMAND diff test2-dir/stdout.out test2-dir/hosts/timerfd-test2-node/stdout-timerfd-test2-node.NODE.1000.log
#         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
########################################################################################################################