#pragma once

#include <windows.h>

class SyncObject
{
public:
	virtual void lock()   = 0;
	virtual void unlock() = 0;
protected:
};

class SingleLock
{
public:
	SingleLock(SyncObject *pSyncObj, bool bInitialLock = true)
	{
		mpSyncObj = pSyncObj;
		if (bInitialLock) {
			lock();
		}
	}

	virtual ~SingleLock()
	{
		unlock();
	}

	void lock()   { mpSyncObj->lock();   }
	void unlock() { mpSyncObj->unlock(); }

protected:
	SyncObject* mpSyncObj;
};

class CriticalSection : public SyncObject
{
public:
	CriticalSection()
	{
		InitializeCriticalSection(&mCritSec);
	}

	~CriticalSection()
	{
		DeleteCriticalSection(&mCritSec);
	}

	virtual void lock()
	{
		EnterCriticalSection(&mCritSec);
	}

	virtual void unlock()
	{
		LeaveCriticalSection(&mCritSec);
	}

protected:
	CRITICAL_SECTION mCritSec; 
};
