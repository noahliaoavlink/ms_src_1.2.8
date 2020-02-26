#ifndef _SmoothCtrl_H_
#define _SmoothCtrl_H_

#define _MAX_HISTORY 5

class SmoothCtrl
{
		float m_fAvgFPS;
		unsigned long m_ulLastTime;
		
		int m_iIntervalOffset;
		float m_fHistory[_MAX_HISTORY];
		int m_iHistoryCount;
		
		bool m_DoSolw;
   protected:
   public:
		SmoothCtrl();
        ~SmoothCtrl();

		void SetAvgFPS(float fValue);
		bool IsAllowed();
		
		bool IsBigHistory(float fValue);
		float GetAvgHistory();
		void ResetHistory();
		void ComputeIntervalOffset(int iValue,int iMax);
		void Reset();
		void EnableSolw(bool bEnable);
};

#endif