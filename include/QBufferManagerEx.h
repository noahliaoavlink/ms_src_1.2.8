#ifndef _QBufferManagerEx_H_
#define _QBufferManagerEx_H_


#define MAX_MQITMES 500

typedef struct
{
	int iSize;
	void* pBuffer;
}MQItem;

class QBufferManagerEx
{
		//unsigned char* m_pBuffer;
		int m_iBufferSize;
		MQItem m_RBMItems[MAX_MQITMES];
//		int m_iTotal;

		int m_iHeaderIndex;
		int m_iLastIndex;
		//unsigned char* m_pLastBuffer;
		int m_iUnitSize;

		int m_iMax;
		unsigned long m_ulID;
		int m_iReadIndex;
		int m_iNextReadIndex;
		int m_iWriteIndex;
		int m_iNextWriteIndex;
		//unsigned char* m_pTempBuffer;

		long m_lFrameCount;
		
		CRITICAL_SECTION m_CriticalSection;
   protected:
   public:
		QBufferManagerEx();
        ~QBufferManagerEx();

		//int Alloc(int iSize);
		void Free();
		bool IsFull();
		bool IsEmpty();
		void Lock();
		void Unlock();
		void SetBufferSize(int iSize);
		void SetMax(int iValue);
		int GetMax();
		//int GetTotal();
		int Insert(int iIndex,unsigned char* pBuffer,int iLen);
		bool Add(void* pBuffer,int iSize);
		int GetNext(unsigned char* pBuffer);

		int GetTotal();
		int GetUnitSize();

		void Reset();
};

#endif