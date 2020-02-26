#ifndef _FrameRateCtrl_H_
#define _FrameRateCtrl_H_

class FrameRateCtrl
{
		float m_fInterval;
   protected:
   public:
		FrameRateCtrl();
        ~FrameRateCtrl();
		
		void SetInterval(float fInterval);
		int GetInterval(int iCount);
};

#endif