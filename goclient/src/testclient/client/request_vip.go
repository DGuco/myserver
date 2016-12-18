package client

import (
	"github.com/golang/protobuf/proto"
	pb "robot/slg/pb"
	"robot/stat"
	"robot/util"
	"time"
)

//7301:上线申请VIP信息
func UserVipInfoReq(robot *Robot) {
	cmd := CMD_USER_VIP_INFO
	req := &pb.UserVipInfoReq{}
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.UserVipInfoRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)
		RequstMng.Dispatch(robot, cmd)
	})
}
