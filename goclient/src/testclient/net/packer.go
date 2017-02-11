package net

import (
	"bytes"
	l4g "code.google.com/p/log4go"
	"encoding/binary"
	"flag"
	"github.com/golang/protobuf/proto"
	"testclient/msg"
	"time"
	"webserver/webconfig"
)

// 包解析器
type Packer struct {
	protocolManager IProtocolManager //协议管理器
}

func (p *Packer) SendMsg(dstFE msg.EnMessageFE, dstID uint32, flag msg.EProxyCmd, pMH *msg.CMessageHead, pMsg proto.Message, id uint64) {
	// CProxyHead
	pPH := &msg.CProxyHead{
		SrcFE:     proto.Uint32(uint32(msg.EnMessageFE_FE_WEBSERVER)),
		SrcID:     proto.Uint32(webconfig.WebCfg.ServerID),
		DstFE:     proto.Uint32(uint32(dstFE)),
		DstID:     proto.Uint32(dstID),
		OpFlag:    flag.Enum(),
		TimeStamp: proto.Uint64(uint64(time.Now().UnixNano() / 1000000)),
	}

	buff := new(bytes.Buffer)

	// 计算长度
	// 总长度
	totalLen := 2

	// 8字节补齐长度
	totalLen += 2

	// CProxyHead长度
	pHeadBin, err := proto.Marshal(pPH)
	if err != nil {
		l4g.Error("CProxyClient: marshal CProxyHead failed, err:%v", err.Error())
		return
	}
	pHeadLen := len(pHeadBin)
	totalLen += 2

	// CProxyHead
	totalLen += pHeadLen

	var pMHBin []byte
	var pMHLen int = 0
	var pMsgBin []byte
	var pMsgLen int = 0

	if pMH != nil && pMsg != nil {
		// CMessageHead长度
		pMHBin, err = proto.Marshal(pMH)
		if err != nil {
			l4g.Error("CProxyClient: marshal CMessageHead failed, err:%v", err.Error())
			return
		}
		pMHLen = len(pMHBin)
		totalLen += 2

		// CMessageHead
		totalLen += pMHLen

		// msg长度
		pMsgBin, err = proto.Marshal(pMsg)
		if err != nil {
			l4g.Error("CProxyClient: marshal msg failed, err: %v", err.Error())
			return
		}
		pMsgLen = len(pMsgBin)
		totalLen += 2

		// msg
		totalLen += pMsgLen
	}

	// 计算8字节对齐补充长度
	addLen := (totalLen % 8)
	if addLen > 0 {
		addLen = (8 - addLen)
	}

	addData := []byte{1, 1, 1, 1, 1, 1, 1}

	totalLen += addLen

	// 将内容写入buff
	binary.Write(buff, binary.LittleEndian, uint16(totalLen))
	binary.Write(buff, binary.LittleEndian, uint16(addLen))
	binary.Write(buff, binary.LittleEndian, uint16(pHeadLen))
	binary.Write(buff, binary.LittleEndian, pHeadBin)
	l4g.Debug("CProxyClient: send CProxyHead:%v", pPH.String())
	if pMH != nil && pMsg != nil {
		binary.Write(buff, binary.LittleEndian, uint16(pMHLen))
		binary.Write(buff, binary.LittleEndian, pMHBin)
		l4g.Debug("CProxyClient: send CMessageHead:%v", pMH.String())
		binary.Write(buff, binary.LittleEndian, uint16(pMsgLen))
		binary.Write(buff, binary.LittleEndian, pMsgBin)
		l4g.Debug("CProxyClient: send msg: %v", pMsg.String())
	}
	binary.Write(buff, binary.LittleEndian, addData[0:addLen])

	// 通知发送数据
	// go p.pushData(id, buff.Bytes())
}

// 解析数据包
func (pk *Packer) Unpack(buffer []byte) []byte {
	// // 判断包头长度
	// bufferLen := uint(len(buffer))
	// headLen := pk.protocolManager.HeadLen()
	// if bufferLen < headLen {
	// 	return buffer
	// }
	// // 解析包头
	// headData := buffer[:headLen]
	// protocol := pk.protocolManager.UnpackProtocol(headData)

	// // 判断包体长度
	// bodyLen := protocol.BodyLen()
	// packetLen := headLen + bodyLen
	// if bufferLen < packetLen {
	// 	return buffer
	// }
	// // 解析包体
	// body := buffer[headLen:packetLen]
	// protocol.SetBody(body)
	// pk.protocolManager.AddProtocol(protocol)

	// // 返回剩余
	// if bufferLen > packetLen {
	// 	return pk.Unpack(buffer[packetLen:])
	// } else {
	// 	return make([]byte, 0)
	// }

	pPH := &msg.CProxyHead{
		SrcFE:     proto.Uint32(uint32(msg.EnMessageFE_FE_WEBSERVER)),
		SrcID:     proto.Uint32(webconfig.WebCfg.ServerID),
		DstFE:     proto.Uint32(uint32(dstFE)),
		DstID:     proto.Uint32(dstID),
		OpFlag:    flag.Enum(),
		TimeStamp: proto.Uint64(uint64(time.Now().UnixNano() / 1000000)),
	}

	buff := new(bytes.Buffer)

	// 计算长度
	// 总长度
	totalLen := 2

	// 8字节补齐长度
	totalLen += 2

	// CProxyHead长度
	pHeadBin, err := proto.Marshal(pPH)
	if err != nil {
		l4g.Error("CProxyClient: marshal CProxyHead failed, err:%v", err.Error())
		return
	}
	pHeadLen := len(pHeadBin)
	totalLen += 2

	// CProxyHead
	totalLen += pHeadLen

	var pMHBin []byte
	var pMHLen int = 0
	var pMsgBin []byte
	var pMsgLen int = 0

	if pMH != nil && pMsg != nil {
		// CMessageHead长度
		pMHBin, err = proto.Marshal(pMH)
		if err != nil {
			l4g.Error("CProxyClient: marshal CMessageHead failed, err:%v", err.Error())
			return
		}
		pMHLen = len(pMHBin)
		totalLen += 2

		// CMessageHead
		totalLen += pMHLen

		// msg长度
		pMsgBin, err = proto.Marshal(pMsg)
		if err != nil {
			l4g.Error("CProxyClient: marshal msg failed, err: %v", err.Error())
			return
		}
		pMsgLen = len(pMsgBin)
		totalLen += 2

		// msg
		totalLen += pMsgLen
	}

	// 计算8字节对齐补充长度
	addLen := (totalLen % 8)
	if addLen > 0 {
		addLen = (8 - addLen)
	}

	addData := []byte{1, 1, 1, 1, 1, 1, 1}

	totalLen += addLen

	// 将内容写入buff
	binary.Write(buff, binary.LittleEndian, uint16(totalLen))
	binary.Write(buff, binary.LittleEndian, uint16(addLen))
	binary.Write(buff, binary.LittleEndian, uint16(pHeadLen))
	binary.Write(buff, binary.LittleEndian, pHeadBin)
	l4g.Debug("CProxyClient: send CProxyHead:%v", pPH.String())
	if pMH != nil && pMsg != nil {
		binary.Write(buff, binary.LittleEndian, uint16(pMHLen))
		binary.Write(buff, binary.LittleEndian, pMHBin)
		l4g.Debug("CProxyClient: send CMessageHead:%v", pMH.String())
		binary.Write(buff, binary.LittleEndian, uint16(pMsgLen))
		binary.Write(buff, binary.LittleEndian, pMsgBin)
		l4g.Debug("CProxyClient: send msg: %v", pMsg.String())
	}
	binary.Write(buff, binary.LittleEndian, addData[0:addLen])

	// 通知发送数据
	go p.pushData(id, buff.Bytes())

}

// 创建解析器实例
func NewPacker(protocolManager IProtocolManager) *Packer {
	packer := new(Packer)
	packer.protocolManager = protocolManager
	return packer
}
