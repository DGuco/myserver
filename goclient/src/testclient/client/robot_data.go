package client

type UserCity struct {
	Castle int         //	主城等级
	Areas  map[int]int //	区域内建筑
}

type RobotData struct {
	userCity UserCity
}
