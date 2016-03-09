#include "stdafx.h"
#include "AutoLock.h"

//////////////////////////////////////////////////////////////////////////
// LockHelper
//////////////////////////////////////////////////////////////////////////
LockHelper::LockHelper()
{
    InitializeCriticalSection(&_crit);
}

LockHelper::~LockHelper()
{
    DeleteCriticalSection(&_crit);
}

void LockHelper::lock()
{
    EnterCriticalSection(&_crit);
}

void LockHelper::unLock()
{
    LeaveCriticalSection(&_crit);
}
