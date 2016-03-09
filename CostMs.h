#pragma once

class CCostMs
{
	public:
		CCostMs();

		void Start();
		__int64 CostTime();

	private:
		_LARGE_INTEGER tm_start;
		_LARGE_INTEGER m_lFeq;
};


