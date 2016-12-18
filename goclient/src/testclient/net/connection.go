package net

// Connect Class

import (
	// "fmt"
	"io"
	"log"
	"net"
	"robot/util"
)

type callback func()

// TCP连接管理类
type TCPConnection struct {
	addr          string
	packer        *Packer
	sendChan      chan []byte
	conn          net.Conn
	connected     bool
	CloseCallback callback
}

// 读协程
func (tc *TCPConnection) readingTask() {
	readBuffer := make([]byte, 1024)
	tempBuffer := make([]byte, 0)
	for tc.connected {

		// 读取字节并解包
		n, err := tc.conn.Read(readBuffer)
		if err == io.EOF {
			log.Println("connect break")
			break
		} else if err != nil {
			util.PrintError(err)
			break
		}
		tempBuffer = tc.packer.Unpack(append(tempBuffer, readBuffer[:n]...))
	}

	tc.Stop()
}

// 写协程
func (tc *TCPConnection) writingTask() {
	for bytes := range tc.sendChan {
		_, err := tc.conn.Write(bytes)
		util.PanicError(err)
	}
}

// 停止任务
func (tc *TCPConnection) Stop() {
	log.Println("close")
	if !tc.connected {
		return
	}
	tc.connected = false
	close(tc.sendChan)
	tc.Close()
}

// 连接并开始收发
func (tc *TCPConnection) Connect() {
	// 建立连接
	conn, err := net.Dial("tcp", tc.addr)
	util.FatalError(err)
	tc.conn = conn
	tc.connected = true

	tc.sendChan = make(chan []byte)

	// 读协程
	go tc.readingTask()

	// 写协程
	go tc.writingTask()

}

// 发送数据
func (tc *TCPConnection) Send(bytes []byte) bool {
	if !tc.connected {
		log.Println("sent data before connected")
		return false
	}
	tc.sendChan <- bytes
	return true
}

// 关闭连接
func (tc *TCPConnection) Close() {
	if tc.conn != nil {
		tc.conn.Close()
		tc.conn = nil

		//tc.CloseCallback()
	}

}

// 创建新TCP连接实例
func NewTCPConnection(addr string, packer *Packer) *TCPConnection {
	return &TCPConnection{addr: addr, packer: packer, connected: false}
}
