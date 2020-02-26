#ifndef SQList_H
#define SQList_H

#include "SList.h" 

class SQList : public SList
{
	   int m_iRef;
	   unsigned long m_ulCurIndex;
	   bool m_bDoInit;
   public:
	   SQList();
	   ~SQList();

	   virtual void* Get(unsigned long ulIndex);
	   virtual void Delete(unsigned long ulIndex);
	   virtual void Reset();
	   virtual unsigned long GetCurIndex();
/*
	   virtual int QueryInterface(int iID,void** ppv);
	   virtual int IncRef();
       virtual int DecRef();
	   */
};

#endif