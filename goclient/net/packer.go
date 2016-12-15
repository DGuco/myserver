package net

// import (
// 	"fmt"
// )

// 包解析器
type Packer struct {
	protocolManager IProtocolManager //协议管理器
}

// 解析数据包
func (pk *Packer) Unpack(buffer []byte) []byte {
	// 判断包头长度
	bufferLen := uint(len(buffer))
	headLen := pk.protocolManager.HeadLen()
	if bufferLen < headLen {
		return buffer
	}
	// 解析包头
	headData := buffer[:headLen]
	protocol := pk.protocolManager.UnpackProtocol(headData)

	// 判断包体长度
	bodyLen := protocol.BodyLen()
	packetLen := headLen + bodyLen
	if bufferLen < packetLen {
		return buffer
	}
	// 解析包体
	body := buffer[headLen:packetLen]
	protocol.SetBody(body)
	pk.protocolManager.AddProtocol(protocol)

	// 返回剩余
	if bufferLen > packetLen {
		return pk.Unpack(buffer[packetLen:])
	} else {
		return make([]byte, 0)
	}
}

// 创建解析器实例
func NewPacker(protocolManager IProtocolManager) *Packer {
	packer := new(Packer)
	packer.protocolManager = protocolManager
	return packer
}
