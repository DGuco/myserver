//
// Created by dguco on 18-8-20.
//

#include <my_assert.h>
#include "file_listener.h"

CFileListener::CFileListener(IEventReactor *pReactor,
							 string filePath,
							 FuncFileListenerOnEvent pFuncNotifyEvent,
							 int flags)
	: m_pReactor(pReactor),
	  m_sFilePath(filePath),
	  m_pFuncOnNotifyEvent(pFuncNotifyEvent),
	  m_iEventFlags(flags)
{
	m_stCallArg = new CCallArg(this);
}

CFileListener::~CFileListener()
{
	if (m_pStBufEv != NULL) {
		bufferevent_free(m_pStBufEv);
	}
	if (m_stCallArg != NULL) {
		delete (m_stCallArg);
		m_stCallArg = NULL;
	}
}

bool CFileListener::RegisterToReactor()
{
	int fd = inotify_init();
	MY_ASSERT_STR(fd > 0, return false, "Inotify_init failed!,error msg: %s", strerror(errno));
	m_oSocket.SetSocket(fd);
	inotify_add_watch(fd, m_sFilePath.c_str(), m_iEventFlags);

	m_pStBufEv = bufferevent_socket_new(GetReactor()->GetEventBase(), fd, 0);
	MY_ASSERT_STR(NULL != m_pStBufEv, return false, "BufferEvent new failed!,error msg: %s", strerror(errno));

	bufferevent_setwatermark(m_pStBufEv, EV_READ, EVENT_SIZE, 0);
	bufferevent_setcb(m_pStBufEv, &CFileListener::lcb_OnRead, NULL, NULL, &m_stCallArg);
	bufferevent_enable(m_pStBufEv, EV_READ);

	return false;
}

IEventReactor *CFileListener::GetReactor()
{
	return m_pReactor;
}

void CFileListener::lcb_OnRead(struct bufferevent *bev, void *arg)
{
	size_t length = 0;
	struct CCallArg *pstr = (struct CCallArg *) arg;
	struct inotify_event *pevent = (struct inotify_event *) pstr->str;

	while (true) {
		length = evbuffer_get_length(bufferevent_get_input(bev));
		if (pstr->m_iLen == 0) {
			if (length < EVENT_SIZE) {
				return;
			}
			pstr->m_iLen += bufferevent_read(bev, pevent, EVENT_SIZE);
		}
		else {
			if (length < pevent->len) {
				printf("Retry body\n");
				return;
			}
			pstr->m_iLen += bufferevent_read(bev, pevent->name, pevent->len);
			pstr->m_iLen = 0;

			/* Done */
			pstr->m_pFileListener->m_pFuncOnNotifyEvent(pevent);
		}
	}
	return;

}
const CSocket &CFileListener::GetSocket() const
{
	return m_oSocket;
}
