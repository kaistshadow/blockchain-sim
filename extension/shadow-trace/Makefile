BUILD_DIR := ${PWD}
DESTDIR := ${BUILD_DIR}/build

all: build
build: install-library build-ltrace shadow-trace

shadow-trace:
	mkdir -p ${DESTDIR}/obj
	gcc -ldl -ldw -o ${DESTDIR}/obj/main.o -c main.cpp -I./modules/ltrace -I./modules/ltrace/sysdeps/ -I./modules/ltrace/sysdeps/linux-gnu -I./modules/ltrace/sysdeps/linux-gnu/x86
	./libtool --tag=CC --mode=link gcc -ldl -ldw -Wall -Wsign-compare -Wfloat-equal -Wformat-security -Werror -g -O2 -o ../bin/shadow-trace ${DESTDIR}/obj/main.o ./modules/ltrace/libltrace.la 

ltrace:
	cd ./modules/ltrace && \
	make
	
build-ltrace:
	cd ./modules/ltrace && \
	./autogen.sh && \
	./configure --without-elfutils && \
	make

install-library: 
	sudo apt install -y libelf-dev libdw-dev libunwind-dev

clean:
	rm -rf ${DESTDIR}