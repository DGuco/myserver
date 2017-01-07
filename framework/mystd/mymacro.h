#ifndef MYMACRO_H_
#define MYMACRO_H_


// 获取数组元素最大个数
#define ARRAY_CNT(a)	int(sizeof(a) / sizeof(a[0]))


// 生成一个1-x范围内的随机数
#define RAND(x) (int)((x)*(rand()/(float)(RAND_MAX + 1.0)))

// --------------------------------------------------------------------------- //
// 定义成员变量并提供set，get方法
// --------------------------------------------------------------------------- //
// 用于定义无符号整形成员变量及set & get方法
#define DEF_UCHAR(name)									\
public:													\
	void Set##name(unsigned char v) {m_uc##name = v;}	\
	unsigned char Get##name() {return m_uc##name;}		\
protected:												\
	unsigned char m_uc##name

#define DEF_USHORT(name)								\
public:													\
	void Set##name(unsigned short v) {m_un##name = v;}	\
	unsigned short Get##name() {return m_un##name;}		\
protected:												\
	unsigned short m_un##name

#define DEF_UINT(name)									\
public:													\
	void Set##name(unsigned int v) {m_ui##name = v;}	\
	unsigned int Get##name() {return m_ui##name;}		\
protected:												\
	unsigned int m_ui##name

#define DEF_ULONG(name)									\
public:													\
	void Set##name(unsigned long v) {m_ul##name = v;}	\
	unsigned long  Get##name() {return m_ul##name;}		\
protected:												\
	unsigned long m_ul##name

#define DEF_ULONGLONG(name)									\
public:														\
	void Set##name(unsigned long long v) {m_ull##name = v;}	\
	unsigned long long Get##name() {return m_ull##name;}	\
protected:													\
	unsigned long long m_ull##name

// 用于定义整形成员变量及set & get方法
#define DEF_CHAR(name)						\
public:										\
	void Set##name(char v) {m_c##name = v;}	\
	char Get##name() {return m_c##name;}	\
protected:									\
	char m_c##name

#define DEF_SHORT(name)							\
public:											\
	void Set##name(short v) {m_n##name = v;}	\
	short Get##name() {return m_n##name;}		\
protected:										\
	short m_n##name

#define DEF_INT(name)						\
public:										\
	void Set##name(int v) {m_i##name = v;}	\
	int  Get##name() {return m_i##name;}	\
protected:									\
	int m_i##name

#define DEF_LONG(name)						\
public:										\
	void Set##name(long v) {m_l##name = v;}	\
	long  Get##name() {return m_l##name;}	\
protected:									\
	long m_l##name

#define DEF_LONGLONG(name)							\
public:												\
	void Set##name(long long v) {m_ll##name = v;}	\
	long long Get##name() {return m_ll##name;}		\
protected:											\
	long long m_ll##name

// 用于定义枚举型成员变量及set & get方法
#define DEF_ENUM(type, name)				\
public:										\
	void Set##name(type v) {m_e##name = v;}	\
	type Get##name() {return m_e##name;}	\
protected:									\
	type m_e##name

// 用于定义指针类型成员变量及set & get方法
#define DEF_PTR(type, name)									\
public:														\
	void Set##name(const type* p) {m_p##name = (type*) p;}	\
	type* Get##name() {return m_p##name;}					\
protected:													\
	type* m_p##name

// 用于定义time_t型成员变量及set & get方法
#define DEF_TIME(name)							\
public:											\
	void Set##name(time_t v) {m_t##name = v;}	\
	time_t Get##name() {return m_t##name;}		\
protected:										\
	time_t m_t##name

// 用于定义size_t型成员变量及set & get方法
#define DEF_SIZE(name)							\
public:											\
	void Set##name(size_t v) {m_s##name = v;}	\
	size_t Get##name() {return m_s##name;}		\
protected:										\
	size_t m_s##name

// 用于定义char[]类型成员变量及set & get方法
#define DEF_ARRAYCHAR(name, capacity)											\
public:																			\
	void Set##name(const char* p)												\
	{																			\
		if (p == NULL)															\
		{																		\
			return;																\
		}																		\
		int iTmpLen = strlen(p);												\
		strncpy(m_ac##name, p, iTmpLen >= capacity ? (capacity - 1) : iTmpLen);	\
	}																			\
	char* Get##name() {return &m_ac##name[0];}									\
protected:																		\
	char m_ac##name[capacity]

// 用于定义无符号整形成员变量及set & get方法
#define DEF_BOOL(name)						\
public:										\
	void Set##name(bool v) {m_b##name = v;}	\
	bool Get##name() {return m_b##name;}	\
protected:									\
	bool m_b##name


// --------------------------------------------------------------------------- //
// 定义成员变量只提供get方法
// --------------------------------------------------------------------------- //
// 用于定义无符号整形成员变量及get方法
#define DEF_UCHAR_ONLYGET(name)							\
public:													\
	unsigned char Get##name() {return m_uc##name;}		\
protected:												\
	unsigned char m_uc##name

#define DEF_USHORT_ONLYGET(name)						\
public:													\
	unsigned short Get##name() {return m_un##name;}		\
protected:												\
	unsigned short m_un##name

#define DEF_UINT_ONLYGET(name)							\
public:													\
	unsigned int Get##name() {return m_ui##name;}		\
protected:												\
	unsigned int m_ui##name

#define DEF_ULONG_ONLYGET(name)							\
public:													\
	unsigned long  Get##name() {return m_ul##name;}		\
protected:												\
	unsigned long m_ul##name

#define DEF_ULONGLONG_ONLYGET(name)							\
public:														\
	unsigned long long Get##name() {return m_ull##name;}	\
protected:													\
	unsigned long long m_ull##name

// 用于定义整形成员变量及get方法
#define DEF_CHAR_ONLYGET(name)				\
public:										\
	char Get##name() {return m_c##name;}	\
protected:									\
	char m_c##name

#define DEF_SHORT_ONLYGET(name)					\
public:											\
	short Get##name() {return m_n##name;}		\
protected:										\
	short m_n##name

#define DEF_INT_ONLYGET(name)				\
public:										\
	int  Get##name() {return m_i##name;}	\
protected:									\
	int m_i##name

#define DEF_LONG_ONLYGET(name)				\
public:										\
	long  Get##name() {return m_l##name;}	\
protected:									\
	long m_l##name

#define DEF_LONGLONG_ONLYGET(name)					\
public:												\
	long long Get##name() {return m_ll##name;}		\
protected:											\
	long long m_ll##name

// 用于定义枚举型成员变量及get方法
#define DEF_ENUM_ONLYGET(type, name)		\
public:										\
	type Get##name() {return m_e##name;}	\
protected:									\
	type m_e##name

// 用于定义指针类型成员变量及get方法
#define DEF_PTR_ONLYGET(type, name)					\
public:												\
	type* Get##name() {return m_p##name;}			\
protected:											\
	type* m_p##name

// 用于定义time_t型成员变量及get方法
#define DEF_TIME_ONLYGET(name)					\
public:											\
	time_t Get##name() {return m_t##name;}		\
protected:										\
	time_t m_t##name

// 用于定义size_t型成员变量及get方法
#define DEF_SIZE_ONLYGET(name)					\
public:											\
	size_t Get##name() {return m_s##name;}		\
protected:										\
	size_t m_s##name

// 用于定义char[]类型成员变量及get方法
#define DEF_ARRAYCHAR_ONLYGET(name, capacity)			\
public:													\
	char* Get##name() {return &m_ac##name[0];}			\
protected:												\
	char m_ac##name[capacity]

// 用于定义无符号整形成员变量及get方法
#define DEF_BOOL_ONLYGET(name)				\
public:										\
	bool Get##name() {return m_b##name;}	\
protected:									\
	bool m_b##name



// 继承CObj的类，定义构造函数，析构函数，初始化函数，resume函数
#define DEFINE_CLASS_TAKEOVER_OBJ_INIT(classname)	\
public:												\
	classname()										\
	{												\
		if (CObj::msCreateMode)						\
		{											\
			Initialize();							\
		}											\
		else										\
		{											\
			Resume();								\
		}											\
	}												\
	~classname() {}									\
	int Initialize();								\
	int Resume();


// 用于objmanager创建实体简化代码
#define CASE_CREATE_OBJ(type, queue)	\
		case type:						\
		{								\
			pTmpObj = queue.create();		\
			break;						\
		}


// 用于objmanager删除实体简化代码
#define CASE_DELETE_OBJ(type, queue)	\
		case type:						\
		{								\
			queue.remove(iObjID);		\
			break;						\
		}


// 用于objmanager获取实体简化代码
#define CASE_GET_OBJ(type, queue)				\
		case type:								\
		{										\
			pTmpObj = queue.find_by_id(iObjID);	\
			break;								\
		}


#endif /* MYMACRO_H_ */
