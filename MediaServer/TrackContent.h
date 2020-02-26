#pragma once
#ifndef _TrackContent_H_
#define _TrackContent_H_

#include "SQList.h"

#define _TIME_CHECK_RANGE 0.8  //for media
#define _TIME_CHECK_RANGE2 0.02  //for number
#define _TIME_CHECK_RANGE3 0.2  //for command
#define _TIME_CHECK_RANGE4 0.9  //for effect index

#define _ENABLE_MEDIA_LEN 1

enum NodeType
{
	NT_NUMBER,
	NT_MEDIA,
	NT_COMMAND,
	NT_EFFECT_INDEX,
	NT_EFFECT,
};

enum LinkType
{
	LT_NONE = 0,
	LT_NORMAL,
	LT_SOAR,
	LT_PLUNGE,
	LT_FLAT_SOAR,
	LT_CURVE,//curve  // GDI+ /DrawBezier,DrawCurve
};

enum CommandType
{
	CT_NONE, //event
	CT_STOP, //stop
	CT_JUMP, //jump
};

enum JumpType
{
	JT_NONE,
	JT_EVENT,
	JT_TIMELINE,
};

typedef struct
{
	float fTime;
	char szPath[512];
#if _ENABLE_MEDIA_LEN
	float fLen;
#endif
}MediaNode;

typedef struct
{
	float fTime;
	int iValue;
	int iLinkType;
	int iAngle;
}NumberNode;

typedef struct
{
	float fTime;
	int iType;  //stop,event,jump
	char szName[128];
	int iJumpType;  //event,timeline
	char szTargetName[128];
}CommandNode;

typedef struct
{
	float fTime;
	int iIndex;
}EffectIndexNode;

typedef struct
{
	char szName[128];
	float fTime;
	int iIndex;    //effect index
	float fDuration;  //secs
	int iLevel;
	int iParam[5];
}EffectNode;

class TrackContent
{
	char m_szName[256];
	int m_iType;

	float m_fTmpTime;

	SQList* m_pNodeList;
protected:
public:
	TrackContent();
	~TrackContent();

	void SetName(char* szData);
	char* GetName();
	void SetType(int iValue);
	int GetType();

	//int Add(float fTime, int iValue, int iLinkType = LT_NORMAL, int iAngle = 0);
	int Add(float fTime, int iValue, int iLinkType, int iAngle = 0);
#if _ENABLE_MEDIA_LEN
	int Add(float fTime, char* szData,float fLen);
#else
	int Add(float fTime, char* szData);
#endif
	int Add(float fTime, int iType, char* szName, char* szTargetName = "", int iJumpType = JT_NONE);
	int Add(float fTime, int iEffectIndex);
	int Add(float fTime, int iEffectIndex, char* szName, float fDuration, int iLevel, int* iParam);
	void Delete(float fTime);
	/*
	int Edit(float fTime, int iValue, int iLinkType = LT_NORMAL, int iAngle = 0);
	int Edit(float fTime, char* szData);
	*/
	int FindNode(float fTime);
	int FindNode_Backward(float fTime);
	void* GetNodeData(int iIndex);
	int GetTotalOfNodes();
	float GetLastTime();
	float GetMaximumTime();
	void Sort();
	int Compare(float fATime, float fBTime);
};

#endif