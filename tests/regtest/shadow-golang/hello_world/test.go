package main

import (
    "C"
   	"fmt"
)

//export test
func test() {
	fmt.Println("Hello world")
}
//export mainGo
func mainGo() {
    test()
}
func main() {
	
}