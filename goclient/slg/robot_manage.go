package slg

import (
	"robot/config"
	"sync"
	"fmt"
	"strconv"
	"log"
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
		AllBuildingUpdata(v)
	}
}

//	将所有建筑物升级到30级
func AllBuildingUpdata (robot *Robot) {
	var buildingItem *config.BuildingItem
	for _, v := range robot.Data.userCity.Areas {
		buildingType := 0
		configTmp := config.BuildingConf.GetItem(v)
		if configTmp != nil {
			buildingType = configTmp.BuildingType
		}

		//log.Println("buildingType:", buildingType)
		if buildingType == 101 {
			buildingItem = configTmp
			log.Println("base level:", buildingItem.Level)
			break
		}
	}
	buildingItemNext := config.BuildingConf.GetItem(buildingItem.UpgradeTo)
	if buildingItemNext != nil {
		//	如果大本等级不到30先把大本升级到30
		for  {
			if buildingItemNext == nil {
				break
			}
			log.Println("base item level", buildingItemNext.Level)
			for bType, bLvl := range buildingItemNext.Condition{
				bTypt, _ := strconv.Atoi(bType)
				UpdateOneBuildingToLevel(robot, bTypt , bLvl)
			}
			BuildingBuild(robot, 4, 9101, 101, 0)
			buildingItemNext = config.BuildingConf.GetItem(buildingItemNext.UpgradeTo)
			if buildingItem.UpgradeTo == 0 {
				break
			}
		}
	}

}

//	把一个类型的建筑建到一个等级 如果没有这个建筑 先建造
func UpdateOneBuildingToLevel (robot *Robot, bType int, bLevel int) {
	log.Println("UpdateOneBuildingToLevel")
	buildingLevel := 0
	buildingAres := 0
	buildingLand := 0
	for k, v := range robot.Data.userCity.Areas {
		configTmp := config.BuildingConf.GetItem(v)
		if configTmp == nil {
			continue
		}
		buildingLand = configTmp.Land
		if configTmp.BuildingType == bType {
			buildingLevel = config.BuildingConf.GetItem(v).Level
			buildingAres = k
			break
		}
	}
	for {
		if buildingLevel <= 0 {
			for k, v := range robot.Data.userCity.Areas {
				if v == 0 {
					areaInfo := config.GridConf.GetItem(k)
					if areaInfo == nil {
						continue
					}
					if areaInfo.GridType != buildingLand {
						continue
					}

					buildingAres = k
					BuildingBuild(robot, 2, buildingAres, bType, 0)
					log.Println("build a new building")
					buildingLevel++
				}
			}
		} else if buildingLevel > 0 && buildingLevel < bLevel{
			BuildingBuild(robot, 4, buildingAres, bType, 0)
			log.Println("build a old building")
			buildingLevel++
		} else {
			break
		}
	}
}