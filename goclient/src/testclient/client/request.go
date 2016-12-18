package client

import (
	"log"
)

const (
	SIGN_BEG = -2
	SIGN_END = -1
)

type requestMethod func(robot *Robot)

type RequestManager struct {
	handlers map[int][]requestMethod
}

func (rm *RequestManager) Init() {
	rm.handlers = make(map[int][]requestMethod)
}

// 绑定状态及下个时间
func (rm *RequestManager) Bind(stat int, handlers ...requestMethod) {
	rm.handlers[stat] = handlers
}

// 状态切换, 请求协议
func (rm *RequestManager) Dispatch(robot *Robot, cmd int) {
	// switch to next request
	handlers, ok := rm.handlers[cmd]
	if !ok {
		//log.Fatalln("not found handler for cmd:", cmd)
		CloseClient(robot)
	}

	for _, handler := range handlers {
		handler(robot)
	}
}

// 开始分发
func (rm *RequestManager) DispatchBegin(robot *Robot) {
	rm.Dispatch(robot, SIGN_BEG)
}

var RequstMng RequestManager

// 结束请求
func CloseClient(robot *Robot) {
	log.Println("robot.GlobalWG.Done()")
	globalWG.Done()
}

// 关联请求流程
func RelateRequests() {
	RequstMng.Init()

	//RequstMng.Bind(SIGN_END, CloseClient)
	RequstMng.Bind(SIGN_BEG, Login)
	//RequstMng.Bind(CMD_LOGIN, RobotTimer, QueryFormation)
	////RequstMng.Bind(CMD_QUERY_FORMATION, QueryArmyStatus)
	//RequstMng.Bind(CMD_QUERY_ARMY_STATUS, QueryTalent)
	//RequstMng.Bind(CMD_QUERY_TALENT, QueryNewbie)
	//RequstMng.Bind(CMD_QUERY_NEWBIE, WarSituation)
	////RequstMng.Bind(CMD_WAR_SITUATION, QueryMapInfo)
	//RequstMng.Bind(CMD_WAR_SITUATION, MarchNpcStatus)
	//RequstMng.Bind(CMD_MARCH_NPC_STATUS, MissionbarInfo)
	//RequstMng.Bind(CMD_MISSION_BAR_INFO, ProtectInfo)
	//RequstMng.Bind(CMD_PROTECT_INFO, EquipmentListReq)
	//RequstMng.Bind(CMD_EQUIPMENT_LIST, SignListReq)
	//RequstMng.Bind(CMD_SIGN_LIST, UserPrivateListReq)
	//RequstMng.Bind(CMD_USER_PRIVATE_LIST, AllianceChatMsgReq)
	//RequstMng.Bind(CMD_ALLIANCE_CHAT, UserVipInfoReq)
	//RequstMng.Bind(CMD_USER_VIP_INFO, AllianceBuildPlusReq)
	//RequstMng.Bind(CMD_ALLIANCE_PLUS, UserRechargeGiftInfoReq)
	//RequstMng.Bind(CMD_USER_GIFT_INFO, UserShowGiftReq)

}
