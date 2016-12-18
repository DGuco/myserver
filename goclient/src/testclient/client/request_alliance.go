package client

import (
	"github.com/golang/protobuf/proto"
	pb "robot/slg/pb"
	"robot/stat"
	"robot/util"
	"time"
)

//2085:联盟加成情况
func AllianceBuildPlusReq(robot *Robot) {
	cmd := CMD_ALLIANCE_PLUS
	req := &pb.AllianceBuildPlusReq{}
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.AllianceBuildPlusAck{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)
		RequstMng.Dispatch(robot, cmd)
	})
}

//2553 拉取联盟聊天信息
func AllianceChatMsgReq(robot *Robot) {
	cmd := CMD_ALLIANCE_CHAT
	req := &pb.AllianceBuildPlusReq{}
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.AllianceChatMsgRsp{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)
		RequstMng.Dispatch(robot, cmd)
	})
}
