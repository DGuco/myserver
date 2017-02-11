package client

import (
	"github.com/golang/protobuf/proto"
	pb "robot/slg/pb"
	"robot/stat"
	"robot/util"
	"time"
)

//4101:请求玩家各种保护状态
func ProtectInfo(robot *Robot) {
	cmd := CMD_PROTECT_INFO
	req := &pb.ProtectInfo{}
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.ProtectInfoRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)
		RequstMng.Dispatch(robot, cmd)
	})
}
