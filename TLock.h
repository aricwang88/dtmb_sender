#ifndef __T_LOCK_H__
#define __T_LOCK_H__

#ifndef WIN32     
#include <pthread.h>
#endif

class TLock
{
//protected:  
    pthread_mutex_t m_pCS;

protected:
#ifdef WIN32     
	CRITICAL_SECTION* m_pCS;
else
#endif

public:
	TLock(void)
	{
#ifdef WIN32     
		m_pCS = new CRITICAL_SECTION();
		InitializeCriticalSection(m_pCS);
#else
        pthread_mutex_init(&m_pCS, NULL);
#endif
        
	};

	virtual ~TLock(void)
	{
#ifdef WIN32
		DeleteCriticalSection(m_pCS);
		delete m_pCS;
#else
        pthread_mutex_destroy(&m_pCS);
#endif
	};

	virtual void Lock(void)
	{
#ifdef WIN32        
		EnterCriticalSection(m_pCS);
#else
        pthread_mutex_lock(&m_pCS);
#endif
	};

	virtual void UnLock(void)
	{
#ifdef WIN32        
		LeaveCriticalSection(m_pCS);
#else
        pthread_mutex_unlock(&m_pCS);
#endif 
	};

};

class ATLock
{
public:
	ATLock(TLock* pTLock)
	{
		m_pTLock = pTLock;

		m_pTLock->Lock();
	};

	virtual ~ATLock()
	{
		m_pTLock->UnLock();
	};

protected:

	TLock* m_pTLock;
};

#endif //__T_LOCK_H__
