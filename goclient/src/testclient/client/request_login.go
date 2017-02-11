package client

import (
	"github.com/golang/protobuf/proto"
	"log"
	pb "robot/slg/pb"
	"robot/stat"
	"robot/util"
	"time"
)

// 登陆协议: 1002
func Login(robot *Robot) {
	cmd := CMD_LOGIN

	// 登陆成功回调函数
	callback := func(data []byte) {
		stat.LoginCount += 1

		res := &pb.LoginResponse{}
		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		robot.Client.uid = res.GetBaseInfo().GetUid()

		robot.Data.userCity.Areas = make(map[int]int)
		for _, v := range res.GetAreas() {
			robot.Data.userCity.Areas[int(v.GetAreaID())] = int(v.GetBuildingID())
		}

		// 解析登陆返回数据
		// 进行下一步操作
		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		stat.ClientLoginData.AfterLogin(duration)

		RequstMng.Dispatch(robot, cmd)
	}

	// 登陆失败回调函数
	anonymous := func(data []byte) {
		res := &pb.VersionLowPush{}

		err := proto.Unmarshal(data, res)
		util.PanicError(err)

		duration := int64(time.Since(robot.Client.sendTime) / time.Millisecond)
		log.Printf("# %s login failed time: %dms\n", robot.Client.openid, duration)
		stat.ClientLoginData.AfterLogin(duration)

	}

	// 硬件信息
	protoEmpty := proto.String("")
	os := int32(1)
	platform := int32(1)
	hardwareinfo := &pb.HardwareInfo{
		ClientVersion:  proto.String(robot.Client.version),
		SystemSoftware: protoEmpty,
		SystemHardware: protoEmpty,
		TelecomOper:    protoEmpty,
		Network:        protoEmpty,
		Density:        protoEmpty,
		UUID:           protoEmpty,
		CpuHardware:    protoEmpty,
		GLRender:       protoEmpty,
		GLVersion:      protoEmpty,
		DeviceId:       protoEmpty,
		LoginChannel:   protoEmpty,
		ScreenWidth:    &platform,
		ScreenHight:    &platform,
		RegChannel:     &platform,
		Memory:         &platform,
	}

	// 发送登陆请求
	req := &pb.LoginRequest{
		Openid:        proto.String(robot.Client.openid),
		Os:            &os,
		Hardware:      hardwareinfo,
		Platform:      &platform,
		AccessToken:   protoEmpty,
		ClientVersion: proto.String(robot.Client.version),
	}

	// 序列化协议
	data, err := proto.Marshal(req)
	util.PanicError(err)

	// 发送协议
	robot.Client.pm.AddCallback(CMD_ANONYMOUS, anonymous)
	robot.Client.Send(cmd, data, callback)
	robot.Client.sendTime = time.Now()
}
