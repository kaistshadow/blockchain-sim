package main

import (
    "C"
   	"fmt"
)

//export test
func test() {
	fmt.Println("Hello World!")
}
//export mainGo
func mainGo() {
    test()
}
func main() {
	
}