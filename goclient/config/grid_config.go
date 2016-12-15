package config

import (
	"os"
	"encoding/json"
	"log"
	"strconv"
)

var GridConf *GridConfig

// 配置文件结构体
type GridConfig struct {
	GridItemAll map[string]*GridItem
}

type GridItem struct {
	ID 		int		`json:"id"`
	GridType	int		`json:"gridType"`
}

// 解析配置文件
func (c *GridConfig) Parser() {
	file, _ := os.Open("data/grid_config.json")
	decoder := json.NewDecoder(file)
	GridConf = &GridConfig{}
	err := decoder.Decode(&GridConf.GridItemAll)
	if err != nil {
		log.Panicln("parser grid_config error:", err)
	}
	log.Println("parser grid_config success")
}

func (c *GridConfig) GetItem (bid int) (gridItem *GridItem) {
	if v, ok := c.GridItemAll[strconv.Itoa(bid)]; ok {
		gridItem = v
	}else {
		gridItem = nil
	}
	return
}