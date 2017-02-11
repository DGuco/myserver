package main

// Main Entry
import (
	"testclient/client"
	"testclient/config"
)

func main() {
	config.ParserConfig()
	client.RelateRequests()
	client.Init()
}
