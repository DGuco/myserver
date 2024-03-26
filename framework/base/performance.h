#ifndef __PERFORMANCE_H__
#define __PERFORMANCE_H__

#include <string>
#include <map>

class CPerfInfo
{
public:
	CPerfInfo();
	~CPerfInfo();

	/// ������ε��þ�����ʱ�䣬���ر����õĴ���
	int AddCalled(int iElapsed);

	/// ����ͳ������
	void Reset();

public:

	int	mPerfLog;		// �Ƿ�ͳ�Ʊ���Ϣ��Ϊ���Ժ����ͳ�����ƶ�.Ĭ��ͳ��
	int	mTotalCalled;	// �����õ��ܴ���
	int	mCostTime;		// �ܹ����ѵ�ʱ��
	int mMostCostTime;	// �����ʱ��
	int mLestCostTime;	// �������ʱ��
};



/// �����Ż�����ֱ��ʹ��string��Ϊmap��key,��װCPerfIndex��Ϊkey
class CPerfIndex
{
public:
	CPerfIndex( unsigned int uiHashCode, const char* pszName ) : 
	  mHashCode( uiHashCode ), mName( pszName ) {}
	~CPerfIndex(){}

	unsigned int	mHashCode;		// ��Ϊ���ȱȽϹؼ���
	std::string		mName;			// perfͳ�Ƶ���Ϣ
};



class CLessPrefIndex
{
public:

	bool operator()(const CPerfIndex& __x, const CPerfIndex& __y) const
	{
		return ( __x.mHashCode < __y.mHashCode )
			|| ( __x.mHashCode == __y.mHashCode && __x.mName < __y.mName );
	}
};


class CPerfStat
{
	typedef std::map<CPerfIndex, CPerfInfo, CLessPrefIndex> PerfMap;
	typedef PerfMap::iterator PerfMapIterator;

	static PerfMap msPerfMap;
	
public:
	/**
	* DJB Hash Function
	* An algorithm produced by Professor Daniel J. Bernstein and shown first to the
	* world on the usenet newsgroup comp.lang.c. It is one of the most efficient
	* hash functions ever published.
	*
	* @param str    ��Ҫ����hashֵ���ַ�����������ASCII 0��β
	*
	* @return �ַ�����Ӧ��hashֵ
	*/
	static inline unsigned int DJBHash(const char* str)
	{
		unsigned int hash = 5381;

		for(const unsigned char* p = reinterpret_cast<const unsigned char*> (str); 0 != *p; ++p)
		{
			hash = ((hash << 5) + hash) + *p;
		}
		return (hash & 0x7FFFFFFF);
	}


	static inline CPerfInfo& GetPerfInfo(const char* name)
	{
		unsigned int hashCode = DJBHash(name);
		CPerfIndex perfIndex(hashCode, name);
		return msPerfMap[perfIndex];
	}

	static inline CPerfInfo& GetPerfInfo(unsigned int hashCode, const char* name)
	{
		CPerfIndex perfIndex(hashCode, name);
		return msPerfMap[perfIndex];
	}

	// ��¼ͳ����Ϣ����־�ļ�
	static void LogPerfInfo();

};



// ͳ�ƺ������ÿ���
#define PERF_FUNC(funcname, callfunc)								\
	do																\
	{																\
		CPerfInfo& info = CPerfStat::GetPerfInfo(funcname);			\
		if (info.mPerfLog)											\
		{															\
			Clock a;												\
			a.start();												\
			callfunc;												\
			info.AddCalled(a.stop());								\
		}															\
		else														\
		{															\
			callfunc;												\
		}															\
	} while (0)

#define PERF_FUNC_RTN( funcname, rtn, callfunc )					\
	do																\
	{																\
		CPerfInfo& info = CPerfStat::GetPerfInfo(funcname);         \
		if (info.mPerfLog)                                          \
		{                                                           \
			Clock a;												\
			a.start();												\
			rtn = callfunc;											\
			info.AddCalled(a.stop());								\
		}															\
		else														\
		{															\
			rtn = callfunc;											\
		}															\
	} while (0)
	

// ���Է�������ʱ���ͳ��
#define PERF_FUNC_ELAPSED(funcname, callfunc, _elapse )				\
		CPerfInfo& info = CPerfStat::GetPerfInfo(funcname);			\
		if (info.mPerfLog)											\
		{															\
			Clock a;												\
			a.start();												\
			callfunc;												\
			info.AddCalled(a.stop());								\
			_elapse = (int)a.elapsed();								\
		}															\
		else														\
		{															\
			callfunc;												\
		}															




// ͳ������д����־
#define PERF_LOG CPerfStat::LogPerfInfo

#define PERF_BEGIN( funcname )													\
		CPerfInfo& funcname##_info = CPerfStat::GetPerfInfo( #funcname );		\
		Clock funcname##_a;														\
		if( funcname##_info.mPerfLog == true )									\
		{																		\
			funcname##_a.start( );												\
		}

#define PERF_END( funcname )													\
		if( funcname##_info.mPerfLog == true)									\
		{																		\
			funcname##_info.AddCalled( funcname##_a.stop( ) );					\
		}

#endif // __PERFORMANCE_H__

