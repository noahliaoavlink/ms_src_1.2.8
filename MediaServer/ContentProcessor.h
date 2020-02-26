#pragma once
#ifndef _ContentProcessor_H_
#define _ContentProcessor_H_

#include "TimeCode.h"
#include "TLTracks.h"
#include "TrackContent.h"

#include "../../../Include/EffectParam.h"
#include "SIniFileW.h"

#define _ENABLE_INTERVAL_MODE 1

using namespace Timeline;

enum TLPlaybackStatus
{
	TL_PS_STOP,
	TL_PS_OPEN,
	TL_PS_PLAY,
};
/*
typedef struct
{
	char szEffectName[128];
	char szParam[6][128];
}EffectParam;

static EffectParam g_effect_param_items[] =
{
	{ "No Effect","","","","","","" },
	{ "Negative","Level", "","","","","" },
	{ "Gray","Level","Contrast","","","","" },
	{ "RGB Color","Level", "Red Shift","Green Shift","Blue Shift","","" },
	{ "Brightness Flicker","Level","Frequency","Amplitude","","","" },
	{ "Sleazy Emboss","Level", "Contast","","","","" },
	{ "Rectangle Bright Dark","Level", "Bright Shift","Left","Right","Top", "Down" },
	{ "Freeze Movie","","Freeze", "","","","" },
	{ "Multi Movie","","Multi", "H Shift","V Shift","Mirror","" },
	{ "Pixelate","","Pixel Size", "","","","" },
	{ "Rotate Scale","","Angle", "Width Zoom","Heigth Zoom","H Shfit","V Shift" },
	{ "Cross Hatching","Level", "Density","","","","" },
	{ "Swirl Texture","","Amount", "Radius","","","" },
	{ "Blind Texture","Level", "Density","Contrast","","","" },
	{ "Neon Light Texture","Level", "Light Size","H Move","V Move","","" },
	{ "Color Modify","Level", "Brightness","Hue","Saturation","","" },
	{ "Glass Windows","Level", "Pattern","","","","" },
	{ "Ripple","","Peak", "Speed","Wavelength","H Shift","V Shift" },
	{ "Fish Eye","","Curvature", "Layer","","","" },
	{ "Halftone","","Density", "Brightness","","","" },
	{ "Wood Noise","","R", "G","B","H Num","V Num" },
	{ "Edge","","Edge Width", "","","","" },
	{ "Wave","","Wave Number", "Frequency","Twist", "","" },
	{ "WaterColor","Level", "Saturation","","","","" },
	{ "Kaleidoscope","Level", "Speed","","","","" },
	{ "Circle Center","","Number", "Angle1","Rotate1","Angle2","Rotate2" },
	{ "Cylinder Rotate","","Number", "Rotate","Shift","","" },
	{ "Plane Rotate","","Plane Num", "Rotate","X Move","Z Move","Distance" },
	{ "Dot Mosaics","","Dot Size", "Brigtness","Soft","","" },
	{ "Single Color","","Color Range", "Red", "Green", "Blue","Saturation" },
	{ "Blur","","Frame Num", "", "","","" },
	{ "Mirage","","Frequency", "Angle", "Range", "Amplitude","Y Offset" }
};
*/

typedef struct
{
	float fTime;
	int iTimeCodeMS;
}LastTriggerNode;

class ContentProcessor
{
	char m_szFullPath[512];
	char m_szEffectName[128];
	char m_szParamName[128];
	TLTrackType m_track_type;
	int m_iTrackX;

	//POINT m_pt1;
	//POINT m_pt2;

	POINT m_pt[6];
	int m_iTotalOfPoints;

	int m_iCurIndex;
	int m_iLastIndex;
	POINT m_ptCurPoint;
	int m_iStreamType;

#ifdef	MULTI_STREAM
	int m_iStreamIndex;
#endif

	int m_iLastMS;
	int m_iLastMS1;
	int m_iLastMS2;
	bool m_bCheckRange;

	int m_iCurMS;  //for debug

	bool m_bUp;
	float m_fDuration;

	int m_iPlaybackStatus;
	float m_fOffset;

	char m_szFileName[512];

	wchar_t wszIniFileName[512];
	SIniFileW* m_pSIniFileW;

	bool m_bSourceFinished;

#ifdef _ENABLE_INTERVAL_MODE
	int m_iLastTimeCodeMS;
	bool m_bUpdateLastTimeCodeMS;
	LastTriggerNode m_LastTriggerNode;
#endif

protected:
public:
	ContentProcessor();
	~ContentProcessor();

	void Init(char* szFullPath, TLTrackType type, int iTrackX);
	void SetFullPath(char* szPath);

	int FindNumberNodes(TimeCode timecode);
	int FindCommandNode(TimeCode timecode);
	int FindCommandNode2(TimeCode last_timecode, TimeCode cur_timecode);
	int FindMediaNode(TimeCode timecode, bool bCheckRange = false);
	int FindMediaNode2(TimeCode last_timecode, TimeCode cur_timecode, bool bCheckRange = false);
	int FindCommandNodebyName(char* szName);
	int FindEffectIndexNode(TimeCode timecode);
	int FindEffectNode(TimeCode timecode);
	void ComputePos(NumberNode* pNumberNode, int& x, int& y);
	void UpdateTimeCode(Gdiplus::Graphics& graphics, TimeCode timecode);
	void UpdateTimeCode(TimeCode timecode);
	int GetCurIndex();
	//POINT GetP1();
	//POINT GetP2();

	POINT GetPoint(int iIndex);
	int GetTotalOfPoints();

	POINT GetIntersectionPoint();
	bool CheckIntersect(POINT p1, POINT p2, int iMS);
	bool CheckCollinear(POINT p1, POINT p2, int iMS);
	int CheckLayerIndex(char* szTrack);
	void PreloadSource();
	void ChangeVideoSource(MediaNode* pNode, float fOffset = 0.0);
	void ChangeMediaEffect();
	int CheckVideoEffect();
	void SetCheckRange(bool bEnable);
	void StopVideoSource();
	void ChangeEffectIndex(int iIndex);

	bool CheckFileStatus();
	void PlayVideoSource();
	bool CheckPBStatus();
	void CloseVideoSource();

	void Write2DecryptFileList(char* szFileName);
	bool CheckFileEncryptStatus(wchar_t* wszFileName);

#ifdef	MULTI_STREAM
	int CheckStreamIndex(char* szTrack);
#endif
	int GetEffectStreamIndex(char* szTrack);
	void ChangeEffect(int iStreamIndex, int iEffectIndex, int iLevel, int* iParam);
	void ResetEffect(int iStreamIndex);
	void ResetEffect();
};

#endif
