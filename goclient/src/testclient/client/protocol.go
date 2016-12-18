package client

import (
	"encoding/binary"
	// "fmt"
	//"log"
	"robot/net"
	"robot/util"
)

const HeadLength = 10 //包头长
const BodyLenExt = 10 //包长扩展，防解密用

// 协议结构体
type Protocol struct {
	bodyLen      uint   //4B:包体长度
	orderNum     int    //2B:序列号
	protoVersion int    //1B:协议版本
	isEncript    int    //1B:是否加密
	cmd          int    //2B:命令字
	body         []byte //{BodyLen}B:包体
}

// 包体长度
func (p *Protocol) BodyLen() uint {
	return p.bodyLen - BodyLenExt
}

// 设置包体内容
func (p *Protocol) SetBody(body []byte) {
	p.body = body
}

type ProtoCallback func(data []byte)

// 协议管理器
type ProtocolManager struct {
	orderNum     uint16
	ProtocolChan chan *Protocol
	callbackMap  map[int]ProtoCallback
}

// 包头长
func (pm *ProtocolManager) HeadLen() uint {
	return HeadLength
}

// 解析协议
func (pm *ProtocolManager) UnpackProtocol(head []byte) net.IProtocol {
	p := new(Protocol)
	p.bodyLen = uint(binary.BigEndian.Uint32(head[:4]))
	p.orderNum = int(binary.BigEndian.Uint16(head[4:6]))
	p.protoVersion = int(head[6])
	p.isEncript = int(head[7])
	p.cmd = int(binary.BigEndian.Uint16(head[8:10]))
	return p
}

// 解析协议
func (pm *ProtocolManager) PackProtocol(cmd int, body []byte) []byte {
	head := make([]byte, HeadLength)
	binary.BigEndian.PutUint32(head[:4], uint32(len(body)+BodyLenExt))
	binary.BigEndian.PutUint16(head[4:6], pm.orderNum)
	head[6] = byte(0)
	head[7] = byte(1)
	binary.BigEndian.PutUint16(head[8:10], uint16(cmd))
	pm.orderNum += 1
	return append(head, body...)
}

// 添加协议
func (pm *ProtocolManager) AddProtocol(protocol net.IProtocol) {
	p, _ := protocol.(*Protocol)
	pm.ProtocolChan <- p
}

// 添加回调函数
func (pm *ProtocolManager) AddCallback(cmd int, callback ProtoCallback) {
	pm.callbackMap[cmd] = callback

}

// 解析protobuf协议
func (pm *ProtocolManager) process() {
	for protocol := range pm.ProtocolChan {
		callback, ok := pm.callbackMap[protocol.cmd]
		if ok {
			var err error
			var result []byte
			if protocol.isEncript != 0 {
				key := []byte("1234567890123456")
				result, err = util.AesDecrypt(protocol.body, key)
				util.PanicError(err)
			} else {
				result = protocol.body
			}
			callback(result)
		} else {
			//log.Println("not found callback of", protocol.cmd)
		}
	}
}

// 构造协议管理器实例
func NewProtocolManager() *ProtocolManager {
	pm := new(ProtocolManager)
	pm.orderNum = 0
	pm.ProtocolChan = make(chan *Protocol)
	pm.callbackMap = make(map[int]ProtoCallback)
	go pm.process()
	return pm
}
