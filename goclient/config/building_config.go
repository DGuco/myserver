package config

import (
	"os"
	"encoding/json"
	"log"
	"strconv"
)

// 全局配置
var BuildingConf *BuildingConfig

// 配置文件结构体
type BuildingConfig struct {
	BuildingItemAll map[string]*BuildingItem
}

type BuildingItem struct {
	ID 		int			`json:"id"`
	Level		int			`json:"level"`
	BuildingType 	int			`json:"buildingType"`
	Output		int			`json:"output"`
	Land		int			`json:"land"`
	PlayerLevel	int			`json:"playerLevel"`
	Condition	map[string]int		`json:"condition"`
	UpgradeTo	int			`json:"upgradeTo"`
	UpgradeView	int			`json:"upgradeView"`
	BuildingExpend	map[string][]int	`json:"buildingExpend"`
}

// 解析配置文件
func (c *BuildingConfig) Parser() {
	file, _ := os.Open("data/building_config.json")
	decoder := json.NewDecoder(file)
	BuildingConf = &BuildingConfig{}
	err := decoder.Decode(&BuildingConf.BuildingItemAll)
	if err != nil {
		log.Panicln("parser building_config error:", err)
	}
	log.Println("parser building_config success")
}

func (c *BuildingConfig) GetItem (bid int) (buildingItem *BuildingItem) {
	if v, ok := c.BuildingItemAll[strconv.Itoa(bid)]; ok {
		buildingItem = v
	}else {
		buildingItem = nil
	}
	return
}