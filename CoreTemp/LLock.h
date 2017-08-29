
#pragma once
#ifndef _MULTITHREAD_H_
#define _MULTITHREAD_H_


/// @brief 手动锁
class LManualLock
{
public:
	/// @brief 构造函数
	LManualLock();

	/// @brief 析构函数
	~LManualLock();

	/// @brief 上锁
	void Lock();

	/// @brief 解锁
	void UnLock();
private:
	LManualLock(const LManualLock&);
	void operator = (const LManualLock&);

	void* m_pLock; ///< 实际锁对象
};

/// @brief 自动锁
class LAutoLock
{
public:
    /// @brief 构造函数
	LAutoLock(LManualLock* pManualLock);

    /// @brief 析构函数
	~LAutoLock();

private:
	LAutoLock(const LAutoLock& );
	void operator = (const LAutoLock&);

	LManualLock* m_pManualLock; ///< 手动锁对象
};
#endif