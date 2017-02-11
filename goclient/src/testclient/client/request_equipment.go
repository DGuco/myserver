package client

import (
	"github.com/golang/protobuf/proto"
	pb "robot/slg/pb"
	"robot/stat"
	"robot/util"
	"time"
)

// 4001 查询酒馆任务状态
func EquipmentListReq(robot *Robot) {
	cmd := CMD_EQUIPMENT_LIST
	reqtype := proto.Int32(1)
	req := &pb.EquipmentListReq{
		Type: reqtype,
	}
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.EquipmentListRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)
		RequstMng.Dispatch(robot, cmd)
	})
}
