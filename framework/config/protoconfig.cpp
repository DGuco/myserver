#include "protoconfig.h"
#include <fcntl.h>
#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>


template<> CConfigMgr* CSingleton<CConfigMgr>::spSingleton = NULL;


int CConfigMgr::LoadConfig(std::string pPathFileName)
{
	m_Config.Clear();

	int tFileDescriptor = open(pPathFileName.c_str(), O_RDONLY);
	if ( tFileDescriptor < 0 )
	{
		LOG_ERROR("default", "[%s : %d : %s] %s open failed.",
				__MY_FILE__, __LINE__, __FUNCTION__, pPathFileName.c_str());
		return -1;
	}
	google::protobuf::io::FileInputStream tFileInput(tFileDescriptor);
	tFileInput.SetCloseOnDelete( true );
	if ( false == google::protobuf::TextFormat::Parse(&tFileInput, &m_Config))
	{
		LOG_ERROR("default", "[%s : %d : %s] %s parse failed.",
				__MY_FILE__, __LINE__, __FUNCTION__, pPathFileName.c_str());
		return -1;
	}

	return 0;
}
