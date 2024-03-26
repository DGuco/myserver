#ifndef RUNFLAG_H_
#define RUNFLAG_H_


// ������״̬
enum ERunFlag
{
	ERF_RUNTIME = 0,	// ������
	ERF_QUIT,			// �˳�
	ERF_RELOAD,			// ���¼�������
};

// ���Ʒ���������״̬����
class CRunFlag
{
public:
	CRunFlag();
	~CRunFlag();

	void SetRunFlag(ERunFlag eRunFlag);
	bool CheckRunFlag(ERunFlag eRunFlag);

protected:
	ERunFlag mRunFlag;
};


#endif /* RUNFLAG_H_ */
