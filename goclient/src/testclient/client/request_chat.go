package client

import (
	"github.com/golang/protobuf/proto"
	pb "robot/slg/pb"
	"robot/stat"
	"robot/util"
	"time"
)

//2303:请求玩家离线私聊信息列表
func UserPrivateListReq(robot *Robot) {
	cmd := CMD_USER_PRIVATE_LIST
	req := &pb.UserPrivateListReq{}
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.UserPrivateListRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)
		RequstMng.Dispatch(robot, cmd)
	})
}
