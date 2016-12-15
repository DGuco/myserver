package slg

import (
	"github.com/golang/protobuf/proto"
	"time"
	pb "robot/slg/pb"
	"robot/util"
	"robot/stat"
)

// 4001 查询酒馆任务状态
func MissionbarInfo(robot *Robot){
	cmd := CMD_MISSION_BAR_INFO
	req := &pb.MissionbarInfo{}
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.MissionbarInfoRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)
		RequstMng.Dispatch(robot, cmd)
	})
}

//7001:签到列表
func SignListReq(robot *Robot){
	cmd := CMD_SIGN_LIST
	req := &pb.SignListReq{}
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.SignListRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)
		RequstMng.Dispatch(robot, cmd)
	})
}
