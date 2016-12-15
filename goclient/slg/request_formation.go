package slg

import (
	"github.com/golang/protobuf/proto"
	pb "robot/slg/pb"
	"robot/util"
)

// 查询天赋: 3002
func QueryTalent(robot *Robot) {
	cmd := CMD_QUERY_TALENT

	// 序列化协议
	req := &pb.QueryTalent{Uid: &robot.Client.uid}
	data, err := proto.Marshal(req)
	util.PanicError(err)

	// 发送协议
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.QueryTalentRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		RequstMng.Dispatch(robot, cmd)
	})
}


// 查询编队: 1407
func QueryFormation(robot *Robot) {
	cmd := CMD_QUERY_FORMATION

	// 序列化协议
	req := &pb.QueryFormation{}
	data, err := proto.Marshal(req)
	util.PanicError(err)

	// 发送协议
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.QueryFormationRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)
		RequstMng.Dispatch(robot, cmd)
	})
}


// 查询军情: 2518
func WarSituation(robot *Robot) {
	cmd := CMD_QUERY_MAP_INFO

	// 发送协议
	robot.Client.Send(cmd, []byte(""), func(data []byte) {
		// 回调函数
		res := &pb.WarSituationPush{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		RequstMng.Dispatch(robot, cmd)
	})
}


// 查询敌方: 1408
func QueryArmyStatus(robot *Robot) {
	cmd := CMD_QUERY_ARMY_STATUS

	// 序列化协议
	req := &pb.QueryArmyStatus{}
	data, err := proto.Marshal(req)
	util.PanicError(err)

	// 发送协议
	robot.Client.Send(cmd, data, func(data []byte) {
		// 回调函数
		res := &pb.QueryArmyStatusRes{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		RequstMng.Dispatch(robot, cmd)
	})
}
