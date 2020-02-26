#ifndef SList_H
#define SList_H

//#include "..\\Interface\\SIList.h"

typedef struct BLink
{
	void* pData;   //data string
	BLink* pNext;  //
}BList;

class SList //: public SIList
{
	   //int m_iRef;

	   BList* m_pHead;
	   BList* m_pTail;
	   BList* m_pLast;
	   BList* m_pCurrent;

	   BList* pTemp;
	   BList* pList;

	   void* m_pData;
	   //int m_iTotal;
	   unsigned long m_ulTotal;
	   bool m_bRemoveData;
	   bool m_bReady;
   public:
	   virtual void Add(void* pData);
       virtual void* Get();
	   virtual void Begin();
	   virtual bool Insert(unsigned long ulIndex,void* pData);
	   //virtual int GetTotal();
	   virtual unsigned long GetTotal();
	   virtual void Delete();    
	   virtual void Reset();
	   virtual void EnableRemoveData(bool bEnable);
	   virtual bool RemoveDataIsEnable();
	   /*
	   virtual int QueryInterface(int iID,void** ppv);
	   virtual int IncRef();
       virtual int DecRef();  
*/
	   SList();
	   ~SList();
};

#endif