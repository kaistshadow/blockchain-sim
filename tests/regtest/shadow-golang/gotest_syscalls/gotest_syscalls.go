package main

import (
    "C"
	"fmt"
	"log"
	"syscall"
)

const filePath = "data.dat"

func test_syscalls() {
	// unlink(2)
	err := syscall.Unlink(filePath)
	if err != nil && err != syscall.ENOENT {
		log.Fatalf("syscall.Unlink: %v\n", err)
	}

	// open(2)
	fd, err := syscall.Open(filePath,
		syscall.O_CREAT|syscall.O_EXCL|syscall.O_RDWR,
		0)
	if err != nil {
		log.Fatalf("syscall.Open: %v\n", err)
	}

	fmt.Printf("fd=%d\n", fd)

	// write(2)
	_, err = syscall.Write(fd, make([]byte, 4096))
	if err != nil {
		log.Fatalf("syscall.Write: %v\n", err)
	}

	// mmap(2)
	data, err := syscall.Mmap(fd, 0, 4096,
		syscall.PROT_READ|syscall.PROT_WRITE,
		syscall.MAP_SHARED)
	if err != nil {
		log.Fatalf("syscall.Mmap: %v\n", err)
	}

	// close(2)
	err = syscall.Close(fd)
	if err != nil {
		log.Fatalf("syscall.Close: %v\n", err)
	}

	fmt.Printf("len(data)=%d\n", len(data))
	fmt.Printf("cap(data)=%d\n", cap(data))

	for i := 0; i < len(data); i++ {
		data[i] = 'x'
	}

	// munmap(2)
	err = syscall.Munmap(data)
	if err != nil {
		log.Fatalf("syscall.Munmap: %v\n", err)
	}

	fmt.Println("Done.")
}

//export mainGo
func mainGo() {
    test_syscalls()
}
func main() {
	
}