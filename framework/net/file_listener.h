//
// Created by dguco on 18-8-20.
// 监听文件变化
//

#ifndef SERVER_FILE_LISTENER_H
#define SERVER_FILE_LISTENER_H

#include <sys/inotify.h>
#include "network_interface.h"
#include "socket.h"

#define EVENT_SIZE  ( sizeof (struct inotify_event) )

class CFileListener;

struct CCallArg
{
public:
	CCallArg(CFileListener *fileListener)
		:
		m_pFileListener(fileListener),
		m_iLen(0)
	{
		memset(str, 0, EVENT_SIZE + 1024);
	}
public:
	CFileListener *m_pFileListener;
	char str[EVENT_SIZE + 1024];
	size_t m_iLen;
};

class CFileListener: public IReactorHandler
{
public:
	/**
	 *
	 * @param pReactor
	 * @param filePath
	 * @param pFuncNotifyEvent
	 * @param flags example:IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO | IN_CLOSE_WRITE
	 */
	CFileListener(IEventReactor *pReactor,
				  string filePath,
				  FuncFileListenerOnEvent pFuncNotifyEvent,
				  int flags);
	virtual ~CFileListener();
	bool RegisterToReactor() override;
	IEventReactor *GetReactor() override;
	const CSocket &GetSocket() const;
public:
	static void lcb_OnRead(struct bufferevent *bev, void *arg);
private:
	IEventReactor *m_pReactor;
	string m_sFilePath;
	FuncFileListenerOnEvent m_pFuncOnNotifyEvent;
	int m_iEventFlags;
	bufferevent *m_pStBufEv;
	CSocket m_oSocket;
	CCallArg *m_stCallArg;
};


#endif //SERVER_FILE_LISTENER_H
