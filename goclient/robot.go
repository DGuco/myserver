package main

// Main Entry

import (
	"robot/config"
	"robot/slg"
)

func main() {
	config.ParserConfig()
	config.BuildingConf.Parser()
	config.GridConf.Parser()
	slg.RelateRequests()

	slg.Init()
}
