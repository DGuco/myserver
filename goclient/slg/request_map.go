package slg

import (
	"github.com/golang/protobuf/proto"
	pb "robot/slg/pb"
	"robot/util"
	"robot/stat"
	"time"
)


// 查询地图信息: 2501
func QueryMapInfo(robot *Robot) {
	cmd := CMD_QUERY_MAP_INFO

	x1 := int32(518)
	y1 := int32(39)
	start := &pb.Position{X: &x1, Y: &y1}

	x2 := int32(528)
	y2 := int32(50)
	end := &pb.Position{X: &x2, Y: &y2}

	x3 := int32(523)
	y3 := int32(45)

	center := &pb.Position{X: &x3, Y: &y3}
	req := &pb.QueryMapInfo{Start: start, End: end, Center: center}

	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)

	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.QueryMapInfoRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)

		RequstMng.Dispatch(robot, cmd)
	})
}

// 查询大地图npc出征状态
func MarchNpcStatus(robot *Robot){
	cmd := CMD_MARCH_NPC_STATUS
	req := &pb.MarchNpcStatus{}
	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)
	// 发送协议
	robot.Client.sendTime = time.Now()
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.MarchNpcStatusPush{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)
		RequstMng.Dispatch(robot, cmd)
	})
}

