package client

import (
	"log"
	"sync"
	"testclient/net"
	"testclient/util"
	"time"
)

type Client struct {
	net.TCPConnection
	uid      int64
	nickname string
	openid   string
	version  string
	sendTime time.Time
	pm       *ProtocolManager

	Data *RobotData
}

// 创建连接
func (cl *Client) Connect() {
	cl.TCPConnection.Connect()
	log.Printf("# %s connect\n", cl.openid)
}

// 协议发送
func (cl *Client) Send(cmd int, data []byte, callback ProtoCallback) {
	key := []byte("1234567890123456")
	result, err := util.AesEncrypt(data, key)
	util.PanicError(err)

	packet := cl.pm.PackProtocol(cmd, result)
	log.Printf("# %s send cmd: %d\n", cl.openid, cmd)
	ok := cl.TCPConnection.Send(packet)
	if ok {
		cl.pm.AddCallback(cmd, callback)
	}
}

func NewClient(wg *sync.WaitGroup, serverAddr string, openid string, ClientVersion string) *Client {
	pm := NewProtocolManager()
	client := new(Client)
	client.TCPConnection = *net.NewTCPConnection(serverAddr, net.NewPacker(pm))
	client.TCPConnection.CloseCallback = func() {
		wg.Done()
	}
	client.openid = openid
	client.nickname = openid
	client.version = ClientVersion
	// 新建协议管理器
	client.pm = pm
	// 创建连接实例
	return client
}
