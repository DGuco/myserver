package slg

import (
	"github.com/golang/protobuf/proto"
	pb "robot/slg/pb"
	"robot/util"
)


// 查询新手: 3006
func QueryNewbie(robot *Robot) {
	cmd := CMD_QUERY_NEWBIE

	// 序列化协议
	req := &pb.QueryNewbid{}
	data, err := proto.Marshal(req)
	util.PanicError(err)

	// 发送协议
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.QueryNewbieRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		RequstMng.Dispatch(robot, cmd)
	})
}
