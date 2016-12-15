package slg

import (
	"robot/config"
)

type Robot struct {
	Data *RobotData
	Client *Client
}

func NewRobot (openid string) *Robot {
	robot := &Robot{}

	client := NewClient(&globalWG, config.Conf.ServerAddr, openid, config.Conf.ClientVersion)
	client.Connect()
	robotData := new(RobotData)
	robot.Client = client
	robot.Data = robotData
	RequstMng.DispatchBegin(robot)

	return robot
}