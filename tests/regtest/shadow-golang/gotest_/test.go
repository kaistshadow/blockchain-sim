package main

import (
    "C"
	"fmt"
	"time"
)

const (
	timeFormat        = "2006-01-02T15:04:05-0700"
	termTimeFormat    = "01-02|15:04:05.000"
	floatFormat       = 'f'
	termMsgJust       = 40
	termCtxMaxPadding = 40
)

func fmt_printf_formatted_time() {
    t := time.Now()
    fmt.Printf("%s %s\n", "hello", t.Format(termTimeFormat))
	fmt.Println("test passed")
}

//export mainGo
func mainGo() {
    fmt_printf_formatted_time();
}

func main() {

}