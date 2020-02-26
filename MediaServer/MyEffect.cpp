#pragma once
#include "stdafx.h"
#include "MyEffect.h"


//class CMyEffect  C_MyEffect;

struct sEffectParam g_EffectParam;

void LOG( char* strMsg, ... )
{
	char err[400]={0};
	va_list args; 	va_start(args, strMsg);	vsnprintf_s( err, 400, 400, strMsg, args );	va_end(args);
	if( ::MessageBoxA( NULL, err, "Exit Program??",  MB_OKCANCEL | MB_TOPMOST) == IDOK) exit(0);
}





