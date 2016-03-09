#include "stdafx.h"
#include "CostMs.h"

void CCostMs::Start()
{
	::QueryPerformanceCounter(&tm_start);
}

__int64 CCostMs::CostTime()
{
	_LARGE_INTEGER tm_end={0,0};
	::QueryPerformanceCounter(&tm_end); 
    __int64 iCostTime = static_cast<__int64>((tm_end.QuadPart - tm_start.QuadPart)*1.0/ m_lFeq.QuadPart *1000);
    
	return iCostTime;
}

CCostMs::CCostMs()
{
	QueryPerformanceFrequency(&m_lFeq);
	Start();
}


