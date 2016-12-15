package util

import (
	"log"
)

func FatalError(err error) {
	if err != nil {
		log.Fatalln(err)
	}
}

func PanicError(err error) {
	if err != nil {
		log.Panicln(err)
	}
}

func PrintError(err error) {
	if err != nil {
		log.Println(err)
	}
}
