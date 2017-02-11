package config

import (
	"encoding/json"
	"flag"
	"log"
	"os"
)

// 全局配置
var Conf *Config

// 配置文件结构体
type Config struct {
	ServerAddr string
	RobotNum   int
}

// 解析配置文件
func (c *Config) Parser(configFile string) {
	file, _ := os.Open(configFile)
	decoder := json.NewDecoder(file)
	err := decoder.Decode(&Conf)
	if err != nil {
		log.Panicln("parser config error:", err)
	}
	log.Println("parser config success")
}

// 控制台输入配置文件地址
var configFile = flag.String("conf", "config.json", "config file")

// 解析配置文件
func ParserConfig() {
	flag.Parse()

	if _, err := os.Stat(*configFile); os.IsNotExist(err) {
		log.Fatalln(err)
	}

	log.Println("load config", *configFile)

	// parser config variables
	Conf.Parser(*configFile)
}
