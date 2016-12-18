package client

import (
	"fmt"
	"log"
	//"strconv"
	"sync"
	"testclient/config"
)

var globalWG sync.WaitGroup

var allRobot map[string]*Robot

func Init() {
	allRobot = make(map[string]*Robot)
	for i := 0; i < config.Conf.RobotNum; i++ {
		globalWG.Add(1)
		openid := fmt.Sprintf("openid%d", i)
		oneRobot := NewRobot(openid)
		allRobot[openid] = oneRobot
	}
	globalWG.Wait()

	AllRobotAction()
}

func AllRobotAction() {
	log.Println("AllRobotAction")
	for _, v := range allRobot {
		log.Println("%v", v)
	}
}
