// https://golang.org/src/syscall/mmap_unix_test.go

package main

import (
    "C"
	"syscall"
	"fmt"
)

func TestMmap() {
	b, err := syscall.Mmap(-1, 0, 4096, syscall.PROT_NONE, syscall.MAP_ANON|syscall.MAP_PRIVATE)
	if err != nil {
		fmt.Printf("well\n")
	}
	b[0] = 'x'
	if err := syscall.Munmap(b); err != nil {
		fmt.Printf("well\n")
	}

}

//export mainGo
func mainGo() {
    TestMmap()
}
func main() {

}