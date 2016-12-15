package stat

import (
	// "fmt"
	"log"
	"time"
)

// 每次打印统计数据的间隔时间
const summaryIntervalSeconds = 3

// 客户登陆结构体
type ClientLogin struct {
	durationChan   chan int64
	maxDuration    int64
	minDuration    int64
	totalDuration  int64
	totalCount     int
	periodDuration int64
	periodCount    int
}

// 虚拟客户登陆成功后调用的处理函数
func (cl *ClientLogin) AfterLogin(duration int64) {
	cl.durationChan <- duration
}

// 客户登陆时长的处理函数
func (cl *ClientLogin) durationProcessor() {
	for duration := range cl.durationChan {
		if duration > cl.maxDuration {
			cl.maxDuration = duration
		}
		if duration < cl.minDuration {
			cl.minDuration = duration
		}
		cl.totalDuration += duration
		cl.totalCount++
		cl.periodCount++
	}
}

// 每间隔固定时间调用数据打印函数
func (cl *ClientLogin) timer() {
	timer := time.NewTicker(time.Second * summaryIntervalSeconds)
	for {
		select {
		case <-timer.C:
			cl.printData()
			cl.printFrequency()
		}
	}
}

// 打印统计数据
func (cl *ClientLogin) printData() {
	if cl.minDuration < 100000.0 {
		log.Printf("# DUR: MAX=%dms, MIN=%dms, AVG=%dms\n",
			cl.maxDuration, cl.minDuration, cl.getAvgDuration())
	}
}

// 打印统计数据
func (cl *ClientLogin) printFrequency() {
	if cl.periodCount == 0 {
		log.Printf("# FRE: 0hz\n")
	} else {
		log.Printf("# FRE: %.1fhz\n", float64(cl.periodCount)/(summaryIntervalSeconds))
		cl.periodCount = 0
	}
}

// 获取平均登陆时长
func (cl *ClientLogin) getAvgDuration() int64 {
	if cl.totalCount != 0 {
		return cl.totalDuration / int64(cl.totalCount)
	} else {
		return 0.0
	}
}

// 初始化客户登陆数据, 并启动对应的协程
func NewClientLogin() *ClientLogin {
	cl := new(ClientLogin)
	cl.durationChan = make(chan int64)
	cl.minDuration = 100000.0
	go cl.durationProcessor()
	go cl.timer()
	return cl
}
