## required for independent build
cmake_minimum_required(VERSION 2.8.8 FATAL_ERROR)

## enable CTEST
enable_testing()

add_custom_target(gnutls-bin ALL
	COMMAND gcc echoserver.c  -o echoserver.out -lgnutls
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(gnutls-shared ALL
	COMMAND gcc echoserver.c -o libechoserv.so -lgnutls -shared -fPIC
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

add_test(NAME shadow-gnutls-exam
	COMMAND shadow simpletest.xml
	WORKING_DIRECTORY ${CMAKE_CUREENT_SOURCE_DIR})
