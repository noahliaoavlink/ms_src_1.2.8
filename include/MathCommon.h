#ifndef _MathCommon_H
#define _MathCommon_H

#define _MAX_ITEMS 1000//30

class Average
{
		int m_iTotalOfItems;
		float m_fValue[_MAX_ITEMS];
		float m_fAvgValue;
		unsigned long m_ulCount;
		
		int m_iMax;
	protected:
    public:
		Average();
		~Average();

		void SetMax(int iValue);
		void Reset();
		void Calculate(float fValue);
		float GetAverage();
		float GetAverage(int iLevel);
		int GetCount();
};

#endif
