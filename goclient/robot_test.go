package main

import (
	"fmt"
	"testing"
)

func TestBinary(t *testing.T) {
	i := 255
	b := byte(i)
	fmt.Printf("#:%X\n", b)
}
