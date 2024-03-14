#include <stdio.h>
#include "log.h"

#include "performance.h"

CPerfInfo::CPerfInfo()
	: mPerfLog(1), mTotalCalled(0), mCostTime(0), mMostCostTime(0), mLestCostTime(1000)
{
	//INIT_ROATING_LOG("Perf", "../log/perf.log", level_enum::info, 10 * 1024 * 1024, 20);
	std::shared_ptr<spdlog::logger> logger = spdlog::create<spdlog::sinks::rotating_file_sink_mt>("Perf", "../log/perf.log", level_enum::info, 10 * 1024 * 1024, 5);
	logger->set_level(level_enum::trace);
	logger->flush_on(level_enum::trace);
}

CPerfInfo::~CPerfInfo()
{
}

/// 输入这次调用经过的时间，返回被调用的次数
int CPerfInfo::AddCalled(int iElapsed)
{

	++mTotalCalled;
	mCostTime += iElapsed;
	if (iElapsed > mMostCostTime) {
		mMostCostTime = iElapsed;
	}
	if (iElapsed < mLestCostTime) {
		mLestCostTime = iElapsed;
	}

	return mTotalCalled;
}

/// 重置统计数据
void CPerfInfo::Reset()
{
	mTotalCalled = 0;
	mCostTime = 0;
	mMostCostTime = 0;
	mLestCostTime = 1000;
}

CPerfStat::PerfMap CPerfStat::msPerfMap;

void CPerfStat::LogPerfInfo()
{
//	INIT_ROLLINGFILE_LOG("Perf", "../log/perf.log", LEVEL_INFO, 10 * 1024 * 1024, 20 );


	static char szTitle[256];
	sprintf(szTitle, "%38s\t%6s\t%6s\t%6s\t%6s\t%6s",
			"name", "called", "cost", "most", "lest", "avg");

	CACHE_LOG(PERF_CACHE, "{}", szTitle);

	PerfMapIterator endi = msPerfMap.end();

	for (PerfMapIterator p = msPerfMap.begin(); p != endi; ++p) {
		char szRecord[256];
		const CPerfIndex &perfIndex = (*p).first;
		CPerfInfo &perfInfo = (*p).second;
		sprintf(szRecord, "%38s\t%6d\t%6d\t%6d\t%6d\t%6d",
				perfIndex.mName.c_str(), perfInfo.mTotalCalled,
				perfInfo.mCostTime, perfInfo.mMostCostTime, perfInfo.mLestCostTime,
				(perfInfo.mTotalCalled != 0) ? perfInfo.mCostTime / perfInfo.mTotalCalled : 0);

		CACHE_LOG(PERF_CACHE, "{}", szRecord);

		perfInfo.Reset();
	}

	msPerfMap.clear();
	CACHE_LOG(PERF_CACHE, "----------------------------------------------------------------------------------");
}
