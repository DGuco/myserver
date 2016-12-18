package net

// 协议结构接口
type IProtocol interface {
	BodyLen() uint  //包体长度
	SetBody([]byte) //设置主体内容
}

// 协议管理器接口
type IProtocolManager interface {
	HeadLen() uint                   //包头
	UnpackProtocol([]byte) IProtocol //解析协议
	AddProtocol(IProtocol)           //添加协议
}
