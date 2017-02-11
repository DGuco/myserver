package client

import (
	"time"
)

// 心跳协议
func HeartBeat(cl *Client) {
	go func() {
		//log.Printf("# %s heartbeat\n", cl.openid)
		// req := &pb.HeartBeat{}
		// data, err := proto.Marshal(req)
		// fmt.Printf("heartbeat data: %T\n", data)
		// util.PanicError(err)
		cl.Send(CMD_HEART_BEAT, make([]byte, 0), func(data []byte) {})
	}()
}

// Timer for every robot
// egg: keep heartbeat with server
func RobotTimer(robot *Robot) {
	go func() {
		timer := time.NewTicker(time.Second * 30)
		for {
			select {
			case <-timer.C:
				HeartBeat(robot.Client)
			}
		}
	}()

}
