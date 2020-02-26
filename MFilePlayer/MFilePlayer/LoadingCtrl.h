#pragma once

#ifndef _LoadingCtrl_H_
#define _LoadingCtrl_H_

class LoadingCtrl
{
	float m_fDataFPS;
	float m_fProcessFPS;

	int m_iFrameCount;

	int m_iQ;     //quotient
	float m_fR;   //remainder
	float m_fR_Num;
protected:
public:
	LoadingCtrl();
	~LoadingCtrl();

	void SetDataFPS(float fFPS);
	void SetProcessFPS(float fFPS);
	bool IsAllowed();
	void Compute();
	void Reset();
};

#endif