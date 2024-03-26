//
//  object.h
//  ��Ϸ������
//  Created by DGuco on 16/12/19.
//  Copyright ? 2016�� DGuco. All rights reserved.
//


#ifndef _OBJECT_H_
#define _OBJECT_H_

#include "base.h"

//OBJ_ID_BITSռλ,ʱ���(4�ֽ�)+����(1�ֽ�)+ͬһ������id(3�ֽ�),ͬһ��ͬ���͵Ķ���ID�ķ�Χ���Ϊ2^^24,
//��ÿ�������16777216��ͬ���Ͷ���(�㹻��) (ע��:������ʱ�䲻�����ص�)
#define OBJ_ID_BITS (24)
#define INVALID_OBJ_ID	0
typedef unsigned long  OBJ_ID;

//ͬһ��id��Χ
#define OBJ_ID_START(type)			(((type) << OBJ_ID_BITS) + 1)
//����Ԥ��10����֤��ͬ����id������ͬ
#define OBJ_ID_END(type)			((((type) + 1) << OBJ_ID_BITS) - 10)

// ����ȡֵ 0 ~ 255 Ϊ�˷�ֹԽ�磬ȥ��ͷβ�߽磬������Чȡֵ��ΧΪ 1 ~ 254,��������ȡֵ��254��0xFE����ʼ�ݼ�
typedef enum _ObjType
{
    OBJ_SESSION_TIMER = 0xFE,
    OBJ_GAMER_TIMER   = 0xFD,
    OBJ_PLAYER_ENTRY  = 0xFC,
    OBJ_INVALID
}EnObjType;

class CObj
{
public:
	CObj()
		/*: id_( 0 )*/
	{
	}

	virtual ~CObj()
	{
		// ��������ʱ������id_�����Ա�֤�ڴ���id_���ݵĺϷ���
		// ������Ϊ��ʱ����Ҫͨ��id���������ݣ����ڹ����ڴ��е����ݣ���������������
		id_ = INVALID_OBJ_ID;
	}

	OBJ_ID get_id() const
	{
		return id_;
	}

	void set_id(OBJ_ID id)
	{
		id_ = id;
	}

	static int ID2TYPE(OBJ_ID id)
	{
		return (id >> OBJ_ID_BITS) & 0X00000000000000FF;
	}

	// �ж�object�Ƿ�Ϸ�
	// ���ڿ����쳣����̫���㣬������������������������캯��ʧ�ܣ�ֻ��Ҫ����id =INVALID_OBJ_ID����
	// ���ַ�ʽ��Ȼ�Ƚϳ���,���ǿ��ԱȽ���Ч�Ĳ�ʹ���쳣�������캯���з��ֵĴ���
	int valid() const
	{
		return INVALID_OBJ_ID != id_;
	}

	// ���ö���Ƿ�
	void invalid()
	{
		id_ = INVALID_OBJ_ID;
	}

	// ��ʼ����������
	virtual int Initialize() = 0;

private:
	OBJ_ID id_; // ���ж����Ψһ��ʶ
};                                               
                                                           
#endif // _OBJECT_H_
