// https://gist.github.com/suyash/a19b7f91000b24fde4bc4a015680c611

package main

import (
    "C"
    "os"
	"syscall"
	"fmt"
)

func TestMmap() {
	    filename, offset := "CMakeLists.txt", 4

    	f, err := os.Open(filename)
    	if err != nil {
    		panic(err)
    	}
    	fd := int(f.Fd())

    	stat, err := f.Stat()
    	if err != nil {
    		panic(err)
    	}
    	size := int(stat.Size())

    	b, err := syscall.Mmap(fd, 0, size, syscall.PROT_READ, syscall.MAP_SHARED)
    	if err != nil {
    		panic(err)
    	}

    	fmt.Printf("%c\n", b[offset-1])

    	err = syscall.Munmap(b)
    	if err != nil {
    		panic(err)
    	}
}

//export mainGo
func mainGo() {
    TestMmap()
    fmt.Printf("syscall.Mmap test passed\n")
}
func main() {

}