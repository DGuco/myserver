package client

import (
	//"robot/stat"
	"github.com/golang/protobuf/proto"
	"log"
	pb "robot/slg/pb"
	"robot/util"
	"time"
)

//cmd1102
//建造建筑物
func BuildingBuild(robot *Robot, option int, areaID int, btype int, cost int) {
	log.Println("BuildingBuild")
	cmd := CMD_BUILDING_BUILD

	req := &pb.Building{}
	req.Option = proto.Int(option)
	req.AreaID = proto.Int(areaID)
	req.Btype = proto.Int(btype)
	req.Cost = proto.Int(cost)
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.BuildingRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		//duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		//stat.ClientLoginData.AfterLogin(duration)
		//RequstMng.Dispatch(robot, cmd)
	})
}
