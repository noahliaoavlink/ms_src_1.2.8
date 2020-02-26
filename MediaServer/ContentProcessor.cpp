#include "stdafx.h"
#include "ContentProcessor.h"
#include "ServiceProvider.h"
#include "../MediaServer/MediaServer.h"
#include "../../../Include/AlgorithmCommon.h"
#include "../../../Include/StrConv.h"
#include "../../../Include/sstring.h"

ContentProcessor::ContentProcessor()
{
	m_iCurIndex = -1;
	m_iLastIndex = -1;
	m_iLastMS = -1;
	m_iLastMS1 = -1;
	m_iLastMS2 = -1;
	m_bCheckRange = false;

#ifdef	MULTI_STREAM
	m_iStreamIndex = 0;
#endif

	m_bUp = false;
	m_fDuration = 0.0;

	m_iPlaybackStatus = TL_PS_STOP;
	m_fOffset = 0.0;

	strcpy(m_szFileName,"");

	m_pSIniFileW = new SIniFileW;

	char szAPPath[512];
	char szFileName[512];
	GetExecutionPath(AfxGetInstanceHandle(), szAPPath);
	sprintf(szFileName, "%s\\decrypt_file_list.txt", szAPPath);
	wcscpy(wszIniFileName, ANSIToUnicode(szFileName));

	m_bSourceFinished = true;

#ifdef _ENABLE_INTERVAL_MODE
	m_iLastTimeCodeMS = 0;
	m_bUpdateLastTimeCodeMS = true;

	m_LastTriggerNode.fTime = 0;
	m_LastTriggerNode.iTimeCodeMS = 0;
#endif
}

ContentProcessor::~ContentProcessor()
{
	if (m_pSIniFileW)
		delete m_pSIniFileW;
}

void ContentProcessor::Init(char* szFullPath, TLTrackType type,int iTrackX)
{
	m_track_type = type;
	m_iTrackX = iTrackX;
	//strcpy(m_sFullPath, szFullPath);
	SetFullPath(szFullPath);
}

void ContentProcessor::SetFullPath(char* szPath)
{
	strcpy(m_szFullPath, szPath);

#ifdef	MULTI_STREAM
	m_iStreamIndex = CheckStreamIndex(m_szFullPath);
#endif

	int iLayerIndex = CheckLayerIndex(m_szFullPath);

	m_iStreamType = -1;
	switch (iLayerIndex)
	{
		case 1:
			m_iStreamType = ST_LAYER1;
			break;
		case 2:
			m_iStreamType = ST_LAYER2;
			break;
		case 3:
			m_iStreamType = ST_LAYER3;
			break;
	}

	char* pch = strrchr(szPath, '/');
	if (pch)
	{
		strcpy(m_szParamName, pch + 1);

		char szTmp[512];
		int iLen = strlen(szPath) - strlen(pch);
		memcpy(szTmp, szPath, iLen);
		szTmp[iLen] = '\0';

		char* pch1 = strrchr(szTmp, '/');
		if(pch1)
			strcpy(m_szEffectName, pch1 + 1);
	}
}

int ContentProcessor::FindNumberNodes(TimeCode timecode)
{
	int iItemIndex, iTrackIndex;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();

	int iMS = timecode.TotalMilliseconds();
	if (m_iLastMS2 != iMS)
		m_iLastMS2 = iMS;
	else
		return -1;

	int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
	if (iRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		for (int i = 0; i < iTotalOfNodes; i++)
		{
			NumberNode* pCurNumberNode = (NumberNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
			if (pCurNumberNode)
			{
				NumberNode* pNextNumberNode = (NumberNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i + 1);

				if (pNextNumberNode)
				{
					if (pCurNumberNode->fTime * 1000 <= iMS && pNextNumberNode->fTime * 1000 >= iMS)
					{
						switch (pNextNumberNode->iLinkType)
						{
							case LT_NORMAL:
								{
									m_iTotalOfPoints = 2;
									ComputePos(pCurNumberNode, (int&)m_pt[0].x, (int&)m_pt[0].y);
									ComputePos(pNextNumberNode, (int&)m_pt[1].x, (int&)m_pt[1].y);

									CheckIntersect(m_pt[0], m_pt[1], iMS);
								}
								break;
							case LT_SOAR:
								{
									m_iTotalOfPoints = 3;
									ComputePos(pCurNumberNode, (int&)m_pt[0].x, (int&)m_pt[0].y);
									ComputePos(pNextNumberNode, (int&)m_pt[2].x, (int&)m_pt[2].y);
									m_pt[1].x = m_pt[0].x;
									m_pt[1].y = m_pt[2].y;

									bool bIsCollinear = CheckCollinear(m_pt[0], m_pt[1],iMS);

									if (bIsCollinear)
									{
										m_ptCurPoint.x = m_pt[1].x;
										m_ptCurPoint.y = m_pt[1].y;
									}
									else
										CheckIntersect(m_pt[1], m_pt[2], iMS);
								}
								break;
							case LT_PLUNGE:
								{
									m_iTotalOfPoints = 3;
									ComputePos(pCurNumberNode, (int&)m_pt[0].x, (int&)m_pt[0].y);
									ComputePos(pNextNumberNode, (int&)m_pt[2].x, (int&)m_pt[2].y);
									m_pt[1].x = m_pt[2].x;
									m_pt[1].y = m_pt[0].y;

									bool bIsCollinear = CheckCollinear(m_pt[1], m_pt[2], iMS);

									if (bIsCollinear)
									{
										m_ptCurPoint.x = m_pt[2].x;
										m_ptCurPoint.y = m_pt[2].y;
									}
									else
										bool bRet = CheckIntersect(m_pt[0], m_pt[1], iMS);
								}
								break;
							case LT_FLAT_SOAR:
								{
									m_iTotalOfPoints = 4;
									ComputePos(pCurNumberNode, (int&)m_pt[0].x, (int&)m_pt[0].y);
									ComputePos(pNextNumberNode, (int&)m_pt[3].x, (int&)m_pt[3].y);

									m_pt[1].x = m_pt[2].x = m_pt[0].x + (m_pt[3].x - m_pt[0].x) / 2;
									m_pt[1].y = m_pt[0].y;
									m_pt[2].y = m_pt[3].y;

									bool bIsCollinear = CheckCollinear(m_pt[1], m_pt[2], iMS);

									if (bIsCollinear)
									{
										m_ptCurPoint.x = m_pt[2].x;
										m_ptCurPoint.y = m_pt[2].y;
									}
									else
									{
										bool bRet = CheckIntersect(m_pt[0], m_pt[1], iMS);
										if(!bRet)
											bRet = CheckIntersect(m_pt[2], m_pt[3], iMS);
									}
								}
								break;
							case LT_CURVE:
								{
									m_iTotalOfPoints = 3;
									ComputePos(pCurNumberNode, (int&)m_pt[0].x, (int&)m_pt[0].y);
									ComputePos(pNextNumberNode, (int&)m_pt[2].x, (int&)m_pt[2].y);

									switch (pNextNumberNode->iAngle)
									{
										case -45:
											{
												m_pt[1].x = m_pt[2].x - (m_pt[2].x - m_pt[0].x) * 0.3;
												m_pt[1].y = m_pt[0].y - (m_pt[0].y - m_pt[2].y) * 0.3;
											}
											break;
										case -90:
											{
												m_pt[1].x = m_pt[2].x - (m_pt[2].x - m_pt[0].x) * 0.07;
												m_pt[1].y = m_pt[0].y - (m_pt[0].y - m_pt[2].y) * 0.07;
											}
											break;
										case 45:
											{
												m_pt[1].x = m_pt[0].x + (m_pt[2].x - m_pt[0].x) * 0.3;
												m_pt[1].y = m_pt[2].y + (m_pt[0].y - m_pt[2].y) * 0.3;
											}
											break;
										case 90:
											{
												m_pt[1].x = m_pt[0].x + (m_pt[2].x - m_pt[0].x) * 0.07;
												m_pt[1].y = m_pt[2].y + (m_pt[0].y - m_pt[2].y) * 0.07;
											}
											break;
									}

									bool bRet = CheckIntersect(m_pt[0], m_pt[1], iMS);
									if (!bRet)
										bRet = CheckIntersect(m_pt[1], m_pt[2], iMS);
								}
								break;
						}

						if (pNextNumberNode->iValue > pCurNumberNode->iValue)
							m_bUp = true;
						else
							m_bUp = false;

						m_fDuration = abs(pNextNumberNode->fTime - pCurNumberNode->fTime);

						return i;
					}
				}
				else
				{
					if (pCurNumberNode->fTime * 1000 == iMS)
					{
						m_iTotalOfPoints = 2;
						ComputePos(pCurNumberNode, (int&)m_pt[0].x, (int&)m_pt[0].y);
						ComputePos(pCurNumberNode, (int&)m_pt[1].x, (int&)m_pt[1].y);
						return i;
					}
				}
			}
		}
	}
	return -1;
}

int ContentProcessor::FindCommandNode(TimeCode timecode)
{
	int iItemIndex, iTrackIndex;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();
	CWnd* pEditViewWnd = manager->GetEditViewWnd();

	int iMS = timecode.TotalMilliseconds();

	char szMsg[512];
	//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d\n", iMS);
	//OutputDebugString(szMsg);

	if (m_iLastMS != iMS)
		m_iLastMS = iMS;
	else
		return -1;

	int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
	if (iRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		for (int i = 0; i < iTotalOfNodes; i++)
		{
			CommandNode* pCurCmdNode = (CommandNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
			if (pCurCmdNode)
			{
				int iStartTime = pCurCmdNode->fTime * 1000;
				int iEndTime = pCurCmdNode->fTime * 1000 + 19;
				//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d [%d , %d](check)\n", iMS, iStartTime, iEndTime);
				//OutputDebugString(szMsg);

				if (pCurCmdNode->fTime * 1000 <= iMS && pCurCmdNode->fTime * 1000 + 19 >= iMS)
				{
#if _ENABLE_INTERVAL_MODE
					if (m_LastTriggerNode.fTime == pCurCmdNode->fTime)
					{
						if (iMS > m_LastTriggerNode.iTimeCodeMS && iMS <= (m_LastTriggerNode.iTimeCodeMS + 19))
							return -1;
					}
#endif

					switch (pCurCmdNode->iType)
					{
						case CT_NONE:
							//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d [%d , %d](CT_NONE)\n", iMS, iStartTime, iEndTime);
							//OutputDebugString(szMsg);
							break;
						case CT_STOP:
							{
								sprintf(szMsg,"ContentProcessor::FindCommandNode - iMS = %d [%d , %d , %s](CT_STOP)\n", iMS, iStartTime, iEndTime, pCurCmdNode->szName);
								OutputDebugString(szMsg);

								auto service = ServiceProvider::Instance()->GetTimelineService();
								if (service != nullptr)
								{
									auto engine = service->GetTimeCodeEngine();
									engine->PauseTimeCodeUpdate();
								}

								PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PAUSE, 0, 0);
							}
							break;
						case CT_JUMP:
							{
								auto time_code_engine = ServiceProvider::Instance()->GetTimelineService()->GetTimeCodeEngine();
								if (pCurCmdNode->iJumpType == JT_EVENT)
								{
									int iIndex = FindCommandNodebyName(pCurCmdNode->szTargetName);
									if (iIndex != -1)
									{
										CommandNode* pTargetCmdNode = (CommandNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
										time_code_engine->SetTimecode(pTargetCmdNode->fTime * 1000);
										//time_code_engine->DoUpdateTimeCode();

										//PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_UPDATE_HAND, pTargetCmdNode->fTime * 1000, 0);
										PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_UPDATE_HAND, pTargetCmdNode->fTime, 0);

#ifdef _ENABLE_INTERVAL_MODE
										m_iLastTimeCodeMS = pTargetCmdNode->fTime * 1000;
										m_bUpdateLastTimeCodeMS = false;
#endif
									}
								}
								else if (pCurCmdNode->iJumpType == JT_TIMELINE)
								{
									manager->ChangeTo(pCurCmdNode->szTargetName);
									time_code_engine->SetTimecode(0);
								}
								//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d [%d , %d](CT_JUMP)\n", iMS, iStartTime, iEndTime);
								//OutputDebugString(szMsg);
							}
							break;
					}

#if _ENABLE_INTERVAL_MODE
					m_LastTriggerNode.fTime = pCurCmdNode->fTime;
					m_LastTriggerNode.iTimeCodeMS = iMS;
#endif
					return i;
				}
			}
		}
	}

	return -1;
}

int ContentProcessor::FindCommandNode2(TimeCode last_timecode,TimeCode cur_timecode)
{
	int iItemIndex, iTrackIndex;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();
	CWnd* pEditViewWnd = manager->GetEditViewWnd();

	int iCurMS = cur_timecode.TotalMilliseconds();
	int iLastMS = last_timecode.TotalMilliseconds();

	char szMsg[512];
	//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d\n", iMS);
	//OutputDebugString(szMsg);

	if (m_iLastMS != iCurMS)
		m_iLastMS = iCurMS;
	else
		return -1;

	int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
	if (iRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		for (int i = 0; i < iTotalOfNodes; i++)
		{
			CommandNode* pCurCmdNode = (CommandNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
			if (pCurCmdNode)
			{
				int iStartTime = pCurCmdNode->fTime * 1000;
				int iEndTime = pCurCmdNode->fTime * 1000 + 19;
				//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d [%d , %d](check)\n", iMS, iStartTime, iEndTime);
				//OutputDebugString(szMsg);

				if (pCurCmdNode->fTime * 1000 >= iLastMS && pCurCmdNode->fTime * 1000 <= iCurMS)
				{
#if _ENABLE_INTERVAL_MODE
					if (m_LastTriggerNode.fTime == pCurCmdNode->fTime)
					{
						if (iCurMS > m_LastTriggerNode.iTimeCodeMS && iCurMS <= (m_LastTriggerNode.iTimeCodeMS + 19))
							return -1;
					}
#endif

					switch (pCurCmdNode->iType)
					{
					case CT_NONE:
						//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d [%d , %d](CT_NONE)\n", iMS, iStartTime, iEndTime);
						//OutputDebugString(szMsg);
						break;
					case CT_STOP:
					{
						sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d [%d , %d , %s](CT_STOP)\n", iCurMS, iStartTime, iEndTime, pCurCmdNode->szName);
						OutputDebugString(szMsg);

						auto service = ServiceProvider::Instance()->GetTimelineService();
						if (service != nullptr)
						{
							auto engine = service->GetTimeCodeEngine();
							engine->PauseTimeCodeUpdate();
						}

						PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_PAUSE, 0, 0);
					}
					break;
					case CT_JUMP:
					{
						auto time_code_engine = ServiceProvider::Instance()->GetTimelineService()->GetTimeCodeEngine();
						if (pCurCmdNode->iJumpType == JT_EVENT)
						{
							int iIndex = FindCommandNodebyName(pCurCmdNode->szTargetName);
							if (iIndex != -1)
							{
								CommandNode* pTargetCmdNode = (CommandNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, iIndex);
								time_code_engine->SetTimecode(pTargetCmdNode->fTime * 1000);
								//time_code_engine->DoUpdateTimeCode();

								//PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_UPDATE_HAND, pTargetCmdNode->fTime * 1000, 0);
								PostMessage(pEditViewWnd->GetSafeHwnd(), WM_TM_UPDATE_HAND, pTargetCmdNode->fTime, 0);

#ifdef _ENABLE_INTERVAL_MODE
								m_iLastTimeCodeMS = pTargetCmdNode->fTime * 1000;
								m_bUpdateLastTimeCodeMS = false;
#endif
							}
						}
						else if (pCurCmdNode->iJumpType == JT_TIMELINE)
						{
							manager->ChangeTo(pCurCmdNode->szTargetName);
							time_code_engine->SetTimecode(0);
						}
						//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d [%d , %d](CT_JUMP)\n", iMS, iStartTime, iEndTime);
						//OutputDebugString(szMsg);
					}
					break;
					}

#if _ENABLE_INTERVAL_MODE
					m_LastTriggerNode.fTime = pCurCmdNode->fTime;
					m_LastTriggerNode.iTimeCodeMS = iCurMS;
#endif
					return i;
				}
			}
		}
	}

	return -1;
}

int ContentProcessor::FindMediaNode(TimeCode timecode,bool bCheckRange)
{
	char szMsg[512];
	int iItemIndex, iTrackIndex;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();

	int iMS = timecode.TotalMilliseconds();

	m_iCurMS = iMS;

	if (m_iLastMS1 != iMS)
		m_iLastMS1 = iMS;
	else
		return -1;

	int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
	if (iRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		for (int i = 0; i < iTotalOfNodes; i++)
		{
			MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
			if (pCurMediaNode)
			{
				if (bCheckRange)
				{
					if (pCurMediaNode->fTime * 1000 <= iMS && (pCurMediaNode->fTime + pCurMediaNode->fLen) * 1000 >= iMS)
					{
						//sprintf(szMsg, "ContentProcessor::FindMediaNode - ChangeVideoSource bCheckRange = %d\n", bCheckRange);
						//OutputDebugString(szMsg);

						float fOffset = (iMS - pCurMediaNode->fTime * 1000) / 1000.0;
						ChangeVideoSource(pCurMediaNode, fOffset);
						return i;
					}
				}
				else
				{
				
#if 0				
					if (pCurMediaNode->fTime * 1000 <= iMS && pCurMediaNode->fTime * 1000 + 20 >= iMS)
					{
						sprintf(szMsg, "#MS [%d] ContentProcessor::FindMediaNode - ChangeVideoSource 0 [%d , %d]\n", m_iStreamIndex , pCurMediaNode->fTime, iMS);
						OutputDebugString(szMsg);

						ChangeVideoSource(pCurMediaNode);
						return i;
					}
					else
					{
						if (pCurMediaNode->fTime * 1000 <= iMS && (pCurMediaNode->fTime + pCurMediaNode->fLen) * 1000 >= iMS)
						{
							bool bIsOpened = CheckFileStatus();
							if (!bIsOpened)
							{
								sprintf(szMsg, "#MS [%d] ContentProcessor::FindMediaNode - ChangeVideoSource 1 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iMS);
								OutputDebugString(szMsg);

								ChangeVideoSource(pCurMediaNode);
							}
						}
					}
#else

					//for demo
					if(pCurMediaNode->fTime * 1000 <= iMS && pCurMediaNode->fTime * 1000 + 19 >= iMS)
					{
						sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::FindMediaNode - ChangeVideoSource 0 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iMS);
						OutputDebugString(szMsg);
						/*
						if (m_LastTriggerNode.fTime == pCurMediaNode->fTime)
						{
							if (iMS > m_LastTriggerNode.iTimeCodeMS && iMS <= (m_LastTriggerNode.iTimeCodeMS + 19))
								return -1;
						}
						*/
						ChangeVideoSource(pCurMediaNode);
						/*
						m_LastTriggerNode.fTime = pCurMediaNode->fTime;
						m_LastTriggerNode.iTimeCodeMS = iMS;
						*/
						return i;
					}
					else
					{
						if(pCurMediaNode->fTime * 1000 + 20 <= iMS && pCurMediaNode->fTime * 1000 + 39 >= iMS)
						{
							bool bUpdate = true;
							if (strcmp(m_szFileName, pCurMediaNode->szPath) == 0)
								bUpdate = false;

							bool bIsOpened = CheckFileStatus();
							if (!bIsOpened || m_iPlaybackStatus == TL_PS_STOP || bUpdate)
							{
								sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::FindMediaNode - ChangeVideoSource 1 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iMS);
								OutputDebugString(szMsg);

								ChangeVideoSource(pCurMediaNode);
								return i;
							}
						}
						else
						{
							if (pCurMediaNode->fTime * 1000 <= iMS && (pCurMediaNode->fTime + pCurMediaNode->fLen) * 1000 >= iMS)
							{
								bool bUpdate = true;
								if (strcmp(m_szFileName, pCurMediaNode->szPath) == 0)
									bUpdate = false;

								bool bIsOpened = CheckFileStatus();
								if (!bIsOpened || m_iPlaybackStatus == TL_PS_STOP || bUpdate)
								{
									sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::FindMediaNode - ChangeVideoSource 2 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iMS);
									OutputDebugString(szMsg);

									ChangeVideoSource(pCurMediaNode);
									return i;
								}
							}
						}
					}

					if (m_iPlaybackStatus == TL_PS_OPEN)
					{
						PlayVideoSource();
					}

#endif
				}
			}
		}
	}
	return -1;
}

int ContentProcessor::FindMediaNode2(TimeCode last_timecode, TimeCode cur_timecode, bool bCheckRange)
{
	int iItemIndex, iTrackIndex;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();

	int iCurMS = cur_timecode.TotalMilliseconds();
	int iLastMS = last_timecode.TotalMilliseconds();

	m_iCurMS = iCurMS;

	char szMsg[512];
	//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d\n", iMS);
	//OutputDebugString(szMsg);

	if (m_iLastMS != iCurMS)
		m_iLastMS = iCurMS;
	else
		return -1;

	int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
	if (iRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		for (int i = 0; i < iTotalOfNodes; i++)
		{
			MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
			if (pCurMediaNode)
			{
				if (bCheckRange)
				{
					if (pCurMediaNode->fTime * 1000 <= iCurMS && (pCurMediaNode->fTime + pCurMediaNode->fLen) * 1000 >= iCurMS)
					{
						//sprintf(szMsg, "ContentProcessor::FindMediaNode - ChangeVideoSource bCheckRange = %d\n", bCheckRange);
						//OutputDebugString(szMsg);

						float fOffset = (iCurMS - pCurMediaNode->fTime * 1000) / 1000.0;
						ChangeVideoSource(pCurMediaNode, fOffset);
						return i;
					}
				}
				else
				{

					if (m_iPlaybackStatus == TL_PS_OPEN)
					{
						PlayVideoSource();
					}

#if _ENABLE_INTERVAL_MODE
					if (pCurMediaNode->fTime * 1000 >= iLastMS && pCurMediaNode->fTime * 1000 <= iCurMS)
					{
						if (m_LastTriggerNode.fTime == pCurMediaNode->fTime)
						{
							if (iCurMS > m_LastTriggerNode.iTimeCodeMS && iCurMS <= (m_LastTriggerNode.iTimeCodeMS + 19))
								return -1;
						}

						sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::FindMediaNode2 - ChangeVideoSource 0 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iCurMS);
						OutputDebugString(szMsg);

						ChangeVideoSource(pCurMediaNode);

						m_LastTriggerNode.fTime = pCurMediaNode->fTime;
						m_LastTriggerNode.iTimeCodeMS = iCurMS;

						return i;
					}
					else if (pCurMediaNode->fTime * 1000 + 20 >= iLastMS && pCurMediaNode->fTime * 1000 + 39 <= iCurMS)
					{
						bool bUpdate = true;
						if (strcmp(m_szFileName, pCurMediaNode->szPath) == 0)
							bUpdate = false;

						bool bIsOpened = CheckFileStatus();
						if (!bIsOpened || m_iPlaybackStatus == TL_PS_STOP || bUpdate)
						{
							sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::FindMediaNode2 - ChangeVideoSource 1 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iCurMS);
							OutputDebugString(szMsg);

							ChangeVideoSource(pCurMediaNode);
							return i;
						}
					}
					else if (pCurMediaNode->fTime * 1000 + 40 >= iLastMS && pCurMediaNode->fTime * 1000 + 59 <= iCurMS)
					{
						bool bUpdate = true;
						if (strcmp(m_szFileName, pCurMediaNode->szPath) == 0)
							bUpdate = false;

						bool bIsOpened = CheckFileStatus();
						if (!bIsOpened || m_iPlaybackStatus == TL_PS_STOP || bUpdate)
						{
							sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::FindMediaNode2 - ChangeVideoSource 2 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iCurMS);
							OutputDebugString(szMsg);

							ChangeVideoSource(pCurMediaNode);
							return i;
						}
					}
					else
					{
						if (pCurMediaNode->fTime * 1000 <= iLastMS && (pCurMediaNode->fTime + pCurMediaNode->fLen) * 1000 >= iCurMS)
						{
							bool bUpdate = true;
							if (strcmp(m_szFileName, pCurMediaNode->szPath) == 0)
								bUpdate = false;

							bool bIsOpened = CheckFileStatus();
							if (!bIsOpened || m_iPlaybackStatus == TL_PS_STOP || bUpdate)
							{
								sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::FindMediaNode2 - ChangeVideoSource 3 [%d , %d] ... \n", m_iStreamIndex, pCurMediaNode->fTime, iCurMS);
								OutputDebugString(szMsg);

								ChangeVideoSource(pCurMediaNode);
								return i;
							}
						}
					}
#else
					//for demo
					if (pCurMediaNode->fTime * 1000 <= iCurMS && pCurMediaNode->fTime * 1000 + 19 >= iCurMS)
					{
						sprintf(szMsg, "#MS [%d] ContentProcessor::FindMediaNode - ChangeVideoSource 0 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iCurMS);
						OutputDebugString(szMsg);
						/*
						if (m_LastTriggerNode.fTime == pCurMediaNode->fTime)
						{
						if (iMS > m_LastTriggerNode.iTimeCodeMS && iMS <= (m_LastTriggerNode.iTimeCodeMS + 19))
						return -1;
						}
						*/
						ChangeVideoSource(pCurMediaNode);
						/*
						m_LastTriggerNode.fTime = pCurMediaNode->fTime;
						m_LastTriggerNode.iTimeCodeMS = iMS;
						*/
						return i;
					}
					else
					{
						if (pCurMediaNode->fTime * 1000 + 20 <= iCurMS && pCurMediaNode->fTime * 1000 + 39 >= iCurMS)
						{
							bool bIsOpened = CheckFileStatus();
							if (!bIsOpened || m_iPlaybackStatus == TL_PS_STOP)
							{
								sprintf(szMsg, "#MS [%d] ContentProcessor::FindMediaNode - ChangeVideoSource 1 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iCurMS);
								OutputDebugString(szMsg);

								ChangeVideoSource(pCurMediaNode);
								return i;
							}
						}
						else
						{
							if (pCurMediaNode->fTime * 1000 <= iCurMS && (pCurMediaNode->fTime + pCurMediaNode->fLen) * 1000 >= iCurMS)
							{
								bool bIsOpened = CheckFileStatus();
								if (!bIsOpened || m_iPlaybackStatus == TL_PS_STOP)
								{
									sprintf(szMsg, "#MS [%d] ContentProcessor::FindMediaNode - ChangeVideoSource 2 [%d , %d]\n", m_iStreamIndex, pCurMediaNode->fTime, iCurMS);
									OutputDebugString(szMsg);

									ChangeVideoSource(pCurMediaNode);
									return i;
								}
							}
						}
					}
#endif
				}
			}
		}
	}
	return -1;
}

int ContentProcessor::FindCommandNodebyName(char* szName)
{
	int iItemIndex, iTrackIndex;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();
	CWnd* pEditViewWnd = manager->GetEditViewWnd();

	//int iMS = timecode.TotalMilliseconds();

	int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
	if (iRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		for (int i = 0; i < iTotalOfNodes; i++)
		{
			CommandNode* pCurCmdNode = (CommandNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
			if (pCurCmdNode)
			{
				if (strcmp(pCurCmdNode->szName, szName) == 0)
					return i;
			}
		}
	}

	return -1;
}

int ContentProcessor::FindEffectIndexNode(TimeCode timecode)
{
	int iItemIndex, iTrackIndex;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();
	CWnd* pEditViewWnd = manager->GetEditViewWnd();

	int iMS = timecode.TotalMilliseconds();

	char szMsg[512];
	//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d\n", iMS);
	//OutputDebugString(szMsg);

	if (m_iLastMS != iMS)
		m_iLastMS = iMS;
	else
		return -1;

	int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
	if (iRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		for (int i = 0; i < iTotalOfNodes; i++)
		{
			EffectIndexNode* pCurInxNode = (EffectIndexNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
			if (pCurInxNode)
			{
				int iStartTime = pCurInxNode->fTime * 1000;
				int iEndTime = pCurInxNode->fTime * 1000 + 19;

				if (pCurInxNode->fTime * 1000 <= iMS && pCurInxNode->fTime * 1000 + 19 >= iMS)
				{
					ChangeEffectIndex(pCurInxNode->iIndex);
					return i;
				}
				else
					ResetEffect();
			}
		}
	}
	return -1;
}

int ContentProcessor::FindEffectNode(TimeCode timecode)
{
	int iItemIndex, iTrackIndex;
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();
	CWnd* pEditViewWnd = manager->GetEditViewWnd();

	int iMS = timecode.TotalMilliseconds();

	char szMsg[512];
	//sprintf(szMsg, "ContentProcessor::FindCommandNode - iMS = %d\n", iMS);
	//OutputDebugString(szMsg);

	if (m_iLastMS != iMS)
		m_iLastMS = iMS;
	else
		return -1;

	int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
	if (iRet != -1)
	{
		int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
		for (int i = 0; i < iTotalOfNodes; i++)
		{
			EffectNode* pCurEffectNode = (EffectNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, i);
			if (pCurEffectNode)
			{
				if (pCurEffectNode->fTime * 1000 <= iMS && (pCurEffectNode->fTime + pCurEffectNode->fDuration) * 1000 >= iMS)
				{
					int iStreamIndex = GetEffectStreamIndex(m_szFullPath);
					ChangeEffect(iStreamIndex, pCurEffectNode->iIndex, pCurEffectNode->iLevel, pCurEffectNode->iParam);
					return i;
				}
				else
				{
					int iStreamIndex = GetEffectStreamIndex(m_szFullPath);
					ResetEffect(iStreamIndex);
				}
			}
		}
	}
	return -1;
}

void ContentProcessor::UpdateTimeCode(Gdiplus::Graphics& graphics, TimeCode timecode)
{
	UpdateTimeCode(timecode);

#ifdef _ENABLE_INTERVAL_MODE
	if (m_bUpdateLastTimeCodeMS)
		m_iLastTimeCodeMS = timecode.TotalMilliseconds();
	else
		m_bUpdateLastTimeCodeMS = true;
#endif
}

void ContentProcessor::UpdateTimeCode(TimeCode timecode)
{
	char szMsg[512];
	int iIndex = -1;

	auto time_code_engine = ServiceProvider::Instance()->GetTimelineService()->GetTimeCodeEngine();
	TimeCode total_time_code = time_code_engine->GetTotalTimecode();

	if (timecode >= total_time_code && total_time_code > 0)
	//if (timecode >= total_time_code + 20 && total_time_code > 0)
	{
		StopVideoSource();
		return;
	}

	if (m_track_type == TLTrackType::Float || m_track_type == TLTrackType::Int)
		iIndex = FindNumberNodes(timecode);
	else if (m_track_type == TLTrackType::Command)
	{

#ifdef _ENABLE_INTERVAL_MODE
		int iMS = timecode.TotalMilliseconds();
		int iCurMS = m_iLastTimeCodeMS;

//		sprintf(szMsg, "ContentProcessor::UpdateTimeCode - m_iLastTimeCodeMS = %d , iMS = %d)\n", m_iLastTimeCodeMS, iMS);
//		OutputDebugString(szMsg);

		if (m_iLastTimeCodeMS < iMS)
			iIndex = FindCommandNode2(m_iLastTimeCodeMS, iMS);
		else
			iIndex = FindCommandNode(timecode);

		if (m_bUpdateLastTimeCodeMS)
			m_iLastTimeCodeMS = iMS;
		else
			m_bUpdateLastTimeCodeMS = true;
#else
		int iMS = timecode.TotalMilliseconds();
		if (iMS - m_iLastMS > 20)
		{
			/*
			char szMsg[512];
			sprintf(szMsg, "ContentProcessor::UpdateTimeCode - iMS - m_iLastMS > 20 (%d ==> %d)\n", m_iLastMS, iMS);
			OutputDebugString(szMsg);
			*/
			FindCommandNode(m_iLastMS + 20);
		}

		iIndex = FindCommandNode(timecode);
#endif
	}
	else if (m_track_type == TLTrackType::Media)
	{
#ifdef _ENABLE_INTERVAL_MODE

		int iMS = timecode.TotalMilliseconds();
		int iCurMS = m_iLastTimeCodeMS;

		TimeCode cur_time_code = TimeCode(iMS);

		if (m_bCheckRange == true)
		{
			m_bCheckRange = false;
			iIndex = FindMediaNode(cur_time_code, true);
		}
		else
		{
			iIndex = FindMediaNode(cur_time_code);
		}
/*
		
		if (m_iLastTimeCodeMS < iMS)
		{
			if (m_bCheckRange == true)
			{
				m_bCheckRange = false;
				iIndex = FindMediaNode2(m_iLastTimeCodeMS, iMS, true);
			}
			else
				iIndex = FindMediaNode2(m_iLastTimeCodeMS, iMS);
		}
		else
		{
			TimeCode cur_time_code = TimeCode(iMS);

			if (m_bCheckRange == true)
			{
				m_bCheckRange = false;
				iIndex = FindMediaNode(cur_time_code, true);
			}
			else
			{
				iIndex = FindMediaNode(cur_time_code);
			}
		}
		*/
		

		if (m_bUpdateLastTimeCodeMS)
			m_iLastTimeCodeMS = iMS;
		else
			m_bUpdateLastTimeCodeMS = true;

		/*
			int iMS = timecode.TotalMilliseconds();

			if (m_bCheckRange == true)
			{
				m_bCheckRange = false;
				iIndex = FindMediaNode(timecode, true);
			}
			else
			{
				if (iMS - m_iLastMS1 > 20)
					iIndex = FindMediaNode(m_iLastMS1 + 20);

				iIndex = FindMediaNode(timecode);
			}
		*/
		
#else
		int iMS = timecode.TotalMilliseconds();

		if (m_bCheckRange == true)
		{
			m_bCheckRange = false;
			iIndex = FindMediaNode(timecode, true);
		}
		else
		{
			if (iMS - m_iLastMS1 > 20)
				iIndex = FindMediaNode(m_iLastMS1 + 20);

			iIndex = FindMediaNode(timecode);
		}
#endif
	}
	else if (m_track_type == TLTrackType::Effect_Index)
		iIndex = FindEffectIndexNode(timecode);
	else if (m_track_type == TLTrackType::Effect)
		iIndex = FindEffectNode(timecode);

	m_iCurIndex = -1;

	if (iIndex != -1)
	{
		m_iCurIndex = iIndex;

		if (m_track_type == TLTrackType::Float || m_track_type == TLTrackType::Int)
		{
			if (m_ptCurPoint.x != -1 && m_ptCurPoint.y != -1)
				ChangeMediaEffect();
		}

		//		Pen pen(Color::Orange, 1);
		//		graphics.DrawLine(&pen, m_pt1.x, m_pt1.y, m_pt2.x, m_pt2.y);
	}
	else
		ResetEffect();
}

void ContentProcessor::ComputePos(NumberNode* pNumberNode,int& x,int& y)
{
	x = pNumberNode->fTime * 100.0 + m_iTrackX;// m_contentRegion.X;

	if (pNumberNode->iValue == 0)
		y = 51;
	else if (pNumberNode->iValue > 0)
	{
		//int iOffset = pNumberNode->iValue / 2;
		int iOffset = 50 - (pNumberNode->iValue / 2);
		y = iOffset;
	}
	else if (pNumberNode->iValue < 0)
	{
		int iOffset = pNumberNode->iValue / 2;
		y = 51 - iOffset;
	}
}

int ContentProcessor::GetCurIndex()
{
	return m_iCurIndex;
}

POINT ContentProcessor::GetPoint(int iIndex)
{
	return m_pt[iIndex];
}

int ContentProcessor::GetTotalOfPoints()
{
	return m_iTotalOfPoints;
}

POINT ContentProcessor::GetIntersectionPoint()
{
	return m_ptCurPoint;
}

bool ContentProcessor::CheckIntersect(POINT p1, POINT p2,int iMS)
{
	CPoint a1(p1.x, p1.y);
	CPoint a2(p2.x, p2.y);

	int iTimeLineX = iMS * 100.0 / 1000.0 + m_iTrackX;
	CPoint b1(iTimeLineX, 0);
	CPoint b2(iTimeLineX, 101);
	CPoint rest_pt = intersection(a1, a2, b1, b2);
	
	m_ptCurPoint.x = rest_pt.x;
	m_ptCurPoint.y = rest_pt.y;

	if (rest_pt.x != -1 && rest_pt.y != -1)
		return true;
	return false;
}

bool ContentProcessor::CheckCollinear(POINT p1, POINT p2, int iMS)
{
	CPoint a1(p1.x, p1.y);
	CPoint a2(p2.x, p2.y);

	int iTimeLineX = iMS * 100.0 / 1000.0 + m_iTrackX;
	CPoint b1(iTimeLineX, 0);
	CPoint b2(iTimeLineX, 101);

	return intersect_collinear(a1, a2, b1, b2);
}

int ContentProcessor::CheckLayerIndex(char* szTrack)
{
	char* pch = strstr(szTrack,"Layer1");
	if (pch)
		return 1;
	pch = strstr(szTrack, "Layer2");
	if (pch)
		return 2;
	pch = strstr(szTrack, "Layer3");
	if (pch)
		return 3;
	pch = strstr(szTrack, "Mixer");
	if (pch)
		return 0;  //main
	return -1;
}

#ifdef	MULTI_STREAM
int ContentProcessor::CheckStreamIndex(char* szTrack)
{
	char* pch = strstr(szTrack, "Source10");
	if (pch)
		return ST_STREAM9;
	pch = strstr(szTrack, "Source2");
	if (pch)
		return ST_STREAM1;
	pch = strstr(szTrack, "Source3");
	if (pch)
		return ST_STREAM2;
	pch = strstr(szTrack, "Source4");
	if (pch)
		return ST_STREAM3;
	pch = strstr(szTrack, "Source5");
	if (pch)
		return ST_STREAM4;
	pch = strstr(szTrack, "Source6");
	if (pch)
		return ST_STREAM5;
	pch = strstr(szTrack, "Source7");
	if (pch)
		return ST_STREAM6;
	pch = strstr(szTrack, "Source8");
	if (pch)
		return ST_STREAM7;
	pch = strstr(szTrack, "Source9");
	if (pch)
		return ST_STREAM8;
	pch = strstr(szTrack, "Source1");
	if (pch)
		return ST_STREAM0;
	return -1;
}
#endif

int ContentProcessor::GetEffectStreamIndex(char* szTrack)
{
	char* pch = strstr(szTrack, "Effect10");
	if (pch)
		return ST_STREAM9;
	pch = strstr(szTrack, "Effect2");
	if (pch)
		return ST_STREAM1;
	pch = strstr(szTrack, "Effect3");
	if (pch)
		return ST_STREAM2;
	pch = strstr(szTrack, "Effect4");
	if (pch)
		return ST_STREAM3;
	pch = strstr(szTrack, "Effect5");
	if (pch)
		return ST_STREAM4;
	pch = strstr(szTrack, "Effect6");
	if (pch)
		return ST_STREAM5;
	pch = strstr(szTrack, "Effect7");
	if (pch)
		return ST_STREAM6;
	pch = strstr(szTrack, "Effect8");
	if (pch)
		return ST_STREAM7;
	pch = strstr(szTrack, "Effect9");
	if (pch)
		return ST_STREAM8;
	pch = strstr(szTrack, "Effect1");
	if (pch)
		return ST_STREAM0;
	return -1;
}

void ContentProcessor::PreloadSource()
{
	if (m_track_type == TLTrackType::Media)
	{

#ifdef _ENABLE_INTERVAL_MODE
		m_iLastTimeCodeMS = 0;
#endif

		int iItemIndex, iTrackIndex;
		auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
		auto track_content_manager = manager->GetTrackContentManager();
		int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
		if (iRet != -1)
		{
			int iTotalOfNodes = track_content_manager->GetTotalOfNodes(iItemIndex, iTrackIndex);
			if (iTotalOfNodes > 0)
			{
				MediaNode* pCurMediaNode = (MediaNode*)track_content_manager->GetNodeData(iItemIndex, iTrackIndex, 0);
				if (pCurMediaNode)
				{
					if (strcmp(m_szFileName, pCurMediaNode->szPath) == 0)
						return;

					OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamIndex);
					pOutDevice->Close();

					char szMsg[512];

					char* szItemName = track_content_manager->GetItemName(iItemIndex);
					pOutDevice->SetGroupName(szItemName);

					sprintf(szMsg, "ContentProcessor::PreloadSource - %d [%s]\n", m_iStreamIndex, m_szFullPath);
					OutputDebugString(szMsg);

					CString strTemp = "";

					if (g_PannelSetting.iEncodeMedia == 1)
					{
						INT32 iIndex = -1;
						for (int y = 0; y < g_MediaStreamAry.size(); y++)
						{
							if (g_MediaStreamAry.at(y)->FilePath().IsEmpty())
							{
								iIndex = y;
								break;
							}
						}

						CFileFind fFind;
						if (!fFind.FindFile(theApp.m_strCurPath + "\\UI Folder\\Temp"))
						{
							CreateDirectory(theApp.m_strCurPath + "\\UI Folder\\Temp", NULL);
						}
						strTemp.Format("%s\\UI Folder\\Temp\\MS%i.mov", theApp.m_strCurPath, iIndex);
#ifdef PLAY_3D_MODE
						CPlaneScene3D::FileDecoder(pCurMediaNode->szPath, strTemp, FALSE);
#endif
					}
					else if (g_PannelSetting.iEncodeMedia == 2)
					{
#ifdef PLAY_3D_MODE
						if (!IsRTSPUrl(pCurMediaNode->szPath))
						{
							wchar_t wszFileName[512];
							wcscpy(wszFileName, ANSIToUnicode(pCurMediaNode->szPath));
							if (CheckFileEncryptStatus(wszFileName))
							{
								strTemp = CPlaneScene3D::FileDecoder(pCurMediaNode->szPath, "", FALSE, TRUE);
								Write2DecryptFileList((char*)strTemp.GetString());
							}
						}
#endif
					}

					if (strTemp.IsEmpty())
						pOutDevice->Open(pCurMediaNode->szPath);
					else
						pOutDevice->Open(strTemp.GetBuffer());

					strcpy(m_szFileName, pCurMediaNode->szPath);
					m_iPlaybackStatus = TL_PS_STOP;
				}
			}
		}
	}
}

void ContentProcessor::ChangeVideoSource(MediaNode* pNode,float fOffset)
{
	char szMsg[1024];

	if (m_iStreamType == -1 && m_iStreamIndex == -1)
		return ;

	if (!m_bSourceFinished)
		return ;

	m_bSourceFinished = false;

	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
#ifdef	MULTI_STREAM
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamIndex);
#else
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamType);
#endif

	pOutDevice->SetStartTime(pNode->fTime);

	if (strcmp(m_szFileName, pNode->szPath) == 0 && pOutDevice->IsOpened())
	{
		sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::ChangeVideoSource 1 [m_szFileName: %s]\n", m_iStreamIndex, m_szFileName);
		OutputDebugString(szMsg);

		sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::ChangeVideoSource 1 [%5.2f , %d , %s]\n", m_iStreamIndex, pNode->fTime, m_iCurMS, pNode->szPath);
		OutputDebugString(szMsg);

		bool bIsClient = false;
		TCAdapter* pTCAdapter = manager->GetTCAdapter();
		if (pTCAdapter)
		{
			if (pTCAdapter->GetMode() == TCM_CLIENT && pTCAdapter->IsWorking())
				bIsClient = true;
		}

		
		//if (!bIsClient)
			pOutDevice->Stop();
		//else
			//pOutDevice->Pause();

		if (!bIsClient)
			pOutDevice->UpdateGlobalTimeCode(0.0);//pOutDevice->Seek(0.0);
		else
		{
		//	if (pTCAdapter)
			//	pTCAdapter->UpdateTimeCodeFromTCClient(0.0);
		}
		pOutDevice->ResetFilter();

#ifdef _ENABLE_GPU
//		pOutDevice->Close();
		//pOutDevice->Open(pNode->szPath);

		//sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::ChangeVideoSource - pOutDevice->Open (0)[%5.2f , %d , %s]\n", m_iStreamIndex, pNode->fTime, m_iCurMS,pNode->szPath);
		//OutputDebugString(szMsg);
#endif

	}
	else
	{
		sprintf(szMsg, "#MS [%d] ContentProcessor::ChangeVideoSource 2\n", m_iStreamIndex);
		OutputDebugString(szMsg);

//		if (m_iPlaybackStatus != TL_PS_STOP)
		{
//			pOutDevice->Stop();
			pOutDevice->Close();
		}

		pOutDevice->ResetFilter();

		CString strTemp = "";

		if (g_PannelSetting.iEncodeMedia == 1)
		{
			INT32 iIndex = -1;
			for (int y = 0; y < g_MediaStreamAry.size(); y++)
			{
				if (g_MediaStreamAry.at(y)->FilePath().IsEmpty())
				{
					iIndex = y;
					break;
				}
			}

			CFileFind fFind;
			if (!fFind.FindFile(theApp.m_strCurPath + "\\UI Folder\\Temp"))
			{
				CreateDirectory(theApp.m_strCurPath + "\\UI Folder\\Temp", NULL);
			}
			strTemp.Format("%s\\UI Folder\\Temp\\MS%i.mov", theApp.m_strCurPath, iIndex);
#ifdef PLAY_3D_MODE
			CPlaneScene3D::FileDecoder(pNode->szPath, strTemp, FALSE);
#endif
		}
		else if (g_PannelSetting.iEncodeMedia == 2)
		{
#ifdef PLAY_3D_MODE
			if (!IsRTSPUrl(pNode->szPath))
			{
				wchar_t wszFileName[512];
				wcscpy(wszFileName, ANSIToUnicode(pNode->szPath));
				if (CheckFileEncryptStatus(wszFileName))
				{
					strTemp = CPlaneScene3D::FileDecoder(pNode->szPath, "", FALSE, TRUE);
					Write2DecryptFileList((char*)strTemp.GetString());
				}
			}
#endif
		}

		int iItemIndex, iTrackIndex;
		auto track_content_manager = manager->GetTrackContentManager();
		int iRet = track_content_manager->GetItemTackIndex(m_szFullPath, iItemIndex, iTrackIndex);
		char* szItemName = track_content_manager->GetItemName(iItemIndex);

		pOutDevice->SetGroupName(szItemName);

		if (strTemp.IsEmpty())
		{
			pOutDevice->Open(pNode->szPath);

			sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::ChangeVideoSource - pOutDevice->Open (1)[%5.2f , %d , %s]\n", m_iStreamIndex, pNode->fTime, m_iCurMS, pNode->szPath);
			OutputDebugString(szMsg);
		}
		else
			pOutDevice->Open(strTemp.GetBuffer());
		//	pOutDevice->Play();
	}

	strcpy(m_szFileName, pNode->szPath);

//	if (fOffset > 0.0)
	//	pOutDevice->Seek(fOffset);

	m_fOffset = fOffset;
	m_iPlaybackStatus = TL_PS_OPEN;

	m_bSourceFinished = true;
}

void ContentProcessor::PlayVideoSource()
{
	char szMsg[512];

	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
#ifdef	MULTI_STREAM
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamIndex);
#else
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamType);
#endif

	pOutDevice->Play();

	sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::PlayVideoSource \n", m_iStreamIndex);
	OutputDebugString(szMsg);

	if (m_fOffset > 0.0)
	{
//		pOutDevice->UpdateGlobalTimeCode(m_fOffset); //pOutDevice->Seek(m_fOffset);
	}

	m_fOffset = 0.0;
	m_iPlaybackStatus = TL_PS_PLAY;
}

void ContentProcessor::StopVideoSource()
{
	char szMsg[512];

	if (m_iStreamType == -1)
		return;

//#ifdef _ENABLE_INTERVAL_MODE
	//m_iLastTimeCodeMS = 0;
//#endif

	char* pch = strstr(m_szFullPath, "Source");
	if (!pch)
		return;

	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
#ifdef	MULTI_STREAM
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamIndex);
#else
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamType);
#endif
	if (pOutDevice)
	{
		
		if (manager->GetTCAdapter()->GetMode() == TCM_CLIENT)
			pOutDevice->Stop2();
		else 
			pOutDevice->Stop();
//		pOutDevice->Close();

		sprintf(szMsg, "#MS [%d] ContentProcessor::StopVideoSource\n", m_iStreamIndex);
		OutputDebugString(szMsg);
	}

	m_iPlaybackStatus = TL_PS_STOP;
}

void ContentProcessor::CloseVideoSource()
{
	char szMsg[512];
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
#ifdef	MULTI_STREAM
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamIndex);
#else
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamType);
#endif
	if (pOutDevice)
	{
		pOutDevice->Close();

		sprintf(szMsg, "#MS_GPU [%d] ContentProcessor::CloseVideoSource\n", m_iStreamIndex);
		OutputDebugString(szMsg);
	}
}

bool ContentProcessor::CheckFileStatus()
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
#ifdef	MULTI_STREAM
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamIndex);
#else
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamType);
#endif
	if (pOutDevice)
	{
		return pOutDevice->IsOpened();
	}
	return false;
}

void ContentProcessor::ChangeMediaEffect()
{
	int iValue = 0;
	if (m_ptCurPoint.y == 51)
		iValue = 0;
	else if (m_ptCurPoint.y > 51)
		iValue = -(m_ptCurPoint.y - 51) * 2;
	else if (m_ptCurPoint.y < 51)
		//iValue = m_ptCurPoint.y * 2;
		iValue = 100 - m_ptCurPoint.y * 2;

	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
#ifdef	MULTI_STREAM
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamIndex);
#else
	OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamType);
#endif

	int iEffectIndex = CheckVideoEffect();
	if (iEffectIndex != -1)
	{
		int iParam[6];

		int iEffectValue = 0;
		if (iValue < 0)// -100 ~ -2
			iEffectValue = 50 + iValue / 2;
		else if (iValue > 0) //0 ~ 100
			iEffectValue = 50 + iValue / 2;
		else if (iValue == 0)
			iEffectValue = 50;

#if 0
		
		for (int i = 0; i < 6; i++)
		{
			iParam[i] = 65535;
			if (strcmp(g_effect_param_items[iEffectIndex].szParam[i], m_szParamName) == 0)
				iParam[i] = iEffectValue;// iValue;
		}

		if(pOutDevice)
			pOutDevice->SetEffectParam(iEffectIndex, iParam[0], iParam[1], iParam[2], iParam[3], iParam[4], iParam[5]);
#else

		for (int i = 0; i < 6; i++)
		{
			iParam[i] = 65535;
		//	if (strcmp("Level", m_szParamName) == 0)
			//	iParam[i] = iEffectValue;// iValue;
		}

		if (strcmp("Level", m_szParamName) == 0)
			iParam[0] = iEffectValue;
		else if (strcmp("Param1", m_szParamName) == 0)
			iParam[1] = iEffectValue;
		else if (strcmp("Param2", m_szParamName) == 0)
			iParam[2] = iEffectValue;
		else if (strcmp("Param3", m_szParamName) == 0)
			iParam[3] = iEffectValue;
		else if (strcmp("Param4", m_szParamName) == 0)
			iParam[4] = iEffectValue;
		else if (strcmp("Param5", m_szParamName) == 0)
			iParam[5] = iEffectValue;

#ifdef	MULTI_STREAM
		for (int i = 0; i < _TL_MAX_LAYER; i++)
		{
			OutDevice* pOutDevice = manager->GetOutDevice(i);
			if (pOutDevice)
			{
				int iCurEffectIndex = pOutDevice->GetEffectIndex();
				/*
				if (strcmp(g_effect_param_items[iCurEffectIndex].szEffectName, "Negative") == 0)
				{
					if(iEffectValue > 50)
						pOutDevice->SetNegative(1);
					else
						pOutDevice->SetNegative(0);
				}
				else if (strcmp(g_effect_param_items[iCurEffectIndex].szEffectName, "Gray") == 0)
				{
					pOutDevice->SetGray(iValue);
				}
				else if (strcmp(g_effect_param_items[iCurEffectIndex].szEffectName, "RGB Color") == 0)
				{
					if (strcmp("Param1", m_szParamName) == 0)
						pOutDevice->SetR(iValue);
					else if (strcmp("Param2", m_szParamName) == 0)
						pOutDevice->SetG(iValue);
					else if (strcmp("Param3", m_szParamName) == 0)
						pOutDevice->SetB(iValue);
				}
				else */
				{
					if (strcmp("Level", m_szParamName) == 0)
						pOutDevice->SetEffectLevel(iParam[0]);
					else if (strcmp("Param1", m_szParamName) == 0)
						pOutDevice->SetEffectParam1(iParam[1]);
					else if (strcmp("Param2", m_szParamName) == 0)
						pOutDevice->SetEffectParam2(iParam[2]);
					else if (strcmp("Param3", m_szParamName) == 0)
						pOutDevice->SetEffectParam3(iParam[3]);
					else if (strcmp("Param4", m_szParamName) == 0)
						pOutDevice->SetEffectParam4(iParam[4]);
					else if (strcmp("Param5", m_szParamName) == 0)
						pOutDevice->SetEffectParam5(iParam[5]);

					//pOutDevice->SetEffectParam(iParam[0], iParam[1], iParam[2], iParam[3], iParam[4], iParam[5]);
				}
			}
		}
#else
		if (pOutDevice)
			pOutDevice->SetEffectParam(iParam[0], iParam[1], iParam[2], iParam[3], iParam[4], iParam[5]);
#endif


#endif
	}
	else
	{
#ifdef	MULTI_STREAM
		for (int i = 0; i < _TL_MAX_LAYER; i++)
		{
			OutDevice* pOutDevice = manager->GetOutDevice(i);
			if (pOutDevice)
			{
				if (strcmp(m_szParamName, "R") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetR(iValue);
				}
				else if (strcmp(m_szParamName, "G") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetG(iValue);
				}
				else if (strcmp(m_szParamName, "B") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetB(iValue);
				}
				else if (strcmp(m_szParamName, "Brightness") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetBrightness(iValue);
				}
				else if (strcmp(m_szParamName, "Hue") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetHue(iValue);
				}
				else if (strcmp(m_szParamName, "Saturation") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetSaturation(iValue);
				}
				else if (strcmp(m_szParamName, "Volumn") == 0)
				{
					int iVolume = 0;
					if (iValue < 0)// -100 ~ -2
						iVolume = 50 + iValue / 2;
					else if (iValue > 0) //0 ~ 100
						iVolume = 50 + iValue / 2;
					else if (iValue == 0)
						iVolume = 50;

					if (pOutDevice)
						pOutDevice->SetVolume(iVolume);
					else
					{
						//layer1
						if (pOutDevice)
							pOutDevice->SetVolume(iVolume);
					}
				}
				else if (strcmp(m_szParamName, "Level") == 0)
				{
					if (strstr(m_szEffectName, "Layer"))
					{
						if (m_iLastIndex != m_iCurIndex)
						{
							m_iLastIndex = m_iCurIndex;

							//layer1
							if (pOutDevice)
							{
								pOutDevice->SetFadeDuration(m_fDuration);

								if (m_bUp)
									pOutDevice->EnableFadeIn(true);
								else
									pOutDevice->EnableFadeOut(true);
							}
							}
						}
					}
			}
		}
#else
		if (strcmp(m_szParamName, "R") == 0)
		{
			if (pOutDevice)
				pOutDevice->SetR(iValue);
		}
		else if (strcmp(m_szParamName, "G") == 0)
		{
			if (pOutDevice)
				pOutDevice->SetG(iValue);
		}
		else if (strcmp(m_szParamName, "B") == 0)
		{
			if (pOutDevice)
				pOutDevice->SetB(iValue);
		}
		else if (strcmp(m_szParamName, "Brightness") == 0)
		{
			if (pOutDevice)
				pOutDevice->SetBrightness(iValue);
		}
		else if (strcmp(m_szParamName, "Hue") == 0)
		{
			if (pOutDevice)
				pOutDevice->SetHue(iValue);
		}
		else if (strcmp(m_szParamName, "Saturation") == 0)
		{
			if (pOutDevice)
				pOutDevice->SetSaturation(iValue);
		}
		else if (strcmp(m_szParamName, "Volumn") == 0)
		{
			int iVolume = 0;
			if (iValue < 0)// -100 ~ -2
				iVolume = 50 + iValue / 2;
			else if (iValue > 0) //0 ~ 100
				iVolume = 50 + iValue / 2;
			else if (iValue == 0)
				iVolume = 50;

			if(pOutDevice)
				pOutDevice->SetVolume(iVolume);
			else
			{
				//layer1
				if(pOutDevice)
					pOutDevice->SetVolume(iVolume);
			}
		}
		else if (strcmp(m_szParamName, "Level") == 0)
		{
			if (strstr(m_szEffectName, "Layer"))
			{
				if (m_iLastIndex != m_iCurIndex)
				{
					m_iLastIndex = m_iCurIndex;

					//layer1
					if (pOutDevice)
					{
						pOutDevice->SetFadeDuration(m_fDuration);

					if (m_bUp)
							pOutDevice->EnableFadeIn(true);
					else
							pOutDevice->EnableFadeOut(true);
					}
				}
			}
		}
#endif

	}
}

int ContentProcessor::CheckVideoEffect()
{
#if 0
	int iTotalOfEffectItems = sizeof(g_effect_param_items) / sizeof(EffectParam);
	for (int i = 0; i < iTotalOfEffectItems; i++)
	{
		if (strcmp(g_effect_param_items[i].szEffectName, m_szEffectName) == 0)
			return i;
	}
	return -1;
#else
	if (strcmp("VideoEffect", m_szEffectName) == 0)
		return 0;
	else
		return -1;
#endif
}

void ContentProcessor::SetCheckRange(bool bEnable)
{
	m_bCheckRange = bEnable;
}

void ContentProcessor::ChangeEffectIndex(int iIndex)
{
#ifdef _ENABLE_HD_EFFECT
	int iTotalOfEffectItems = sizeof(g_hd_effect_param_items) / sizeof(EffectParam);
#else
	int iTotalOfEffectItems = sizeof(g_effect_param_items) / sizeof(EffectParam);
#endif

	if (iIndex < iTotalOfEffectItems)
	{
		auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
#ifdef _ENABLE_SINGLE_EFFECT
		manager->GetDisplayManager()->SetEffectIndex(iIndex);

#ifdef	MULTI_STREAM
		for (int i = 0; i < _TL_MAX_LAYER; i++)
		{
			OutDevice* pOutDevice = manager->GetOutDevice(i);
			if (pOutDevice)
				pOutDevice->SetEffectIndex(iIndex);
		}
#else
		OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamType);
		pOutDevice->SetEffectIndex(iIndex);
#endif

#else    //_ENABLE_SINGLE_EFFECT

#ifdef	MULTI_STREAM
		for (int i = 0; i < _TL_MAX_LAYER; i++)
		{
			OutDevice* pOutDevice = manager->GetOutDevice(i);
			if(pOutDevice)
				pOutDevice->SetEffectIndex(iIndex);
		}
#else
		OutDevice* pOutDevice = manager->GetOutDevice(m_iStreamType);
		pOutDevice->SetEffectIndex(iIndex);
#endif

#endif  _ENABLE_SINGLE_EFFECT
		manager->UpdateVideoEffectParamItem(iIndex);
	}
}

bool ContentProcessor::CheckPBStatus()
{
	if (m_iPlaybackStatus == TL_PS_OPEN || m_iPlaybackStatus == TL_PS_PLAY)
		return true;
	return false;
}

void ContentProcessor::ResetEffect()
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	int iParam[6];

	int iEffectIndex = CheckVideoEffect();
	if (iEffectIndex != -1)
	{
		/*
		for (int i = 0; i < 6; i++)
		{
			iParam[i] = 65535;
		}
		*/
		for (int i = 0; i < _TL_MAX_LAYER; i++)
		{
			OutDevice* pOutDevice = manager->GetOutDevice(i);
			if (pOutDevice)
			{
				int iCurEffectIndex = pOutDevice->GetEffectIndex();
				
				if(iCurEffectIndex > 0)
				{
					if (strcmp("Level", m_szParamName) == 0)
						pOutDevice->SetEffectLevel(0);
				}
			}
		}
	}
	else
	{
		int iValue = 0;
		for (int i = 0; i < _TL_MAX_LAYER; i++)
		{
			OutDevice* pOutDevice = manager->GetOutDevice(i);
			if (pOutDevice)
			{
				if (strcmp(m_szParamName, "R") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetR(iValue);
				}
				else if (strcmp(m_szParamName, "G") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetG(iValue);
				}
				else if (strcmp(m_szParamName, "B") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetB(iValue);
				}
				else if (strcmp(m_szParamName, "Brightness") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetBrightness(iValue);
				}
				else if (strcmp(m_szParamName, "Hue") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetHue(iValue);
				}
				else if (strcmp(m_szParamName, "Saturation") == 0)
				{
					if (pOutDevice)
						pOutDevice->SetSaturation(iValue);
				}
				else if (strcmp(m_szParamName, "Volumn") == 0)
				{
					/*
					if (pOutDevice)
						pOutDevice->SetVolume(0);
					else
					{
						//layer1
						if (pOutDevice)
							pOutDevice->SetVolume(0);
					}
					*/
				}
				else if (strcmp(m_szParamName, "Level") == 0)
				{
					if (strstr(m_szEffectName, "Layer"))
					{
						if (m_iLastIndex != m_iCurIndex)
						{
							m_iLastIndex = m_iCurIndex;

							//layer1
							if (pOutDevice)
							{
								pOutDevice->SetFadeDuration(m_fDuration);

								if (m_bUp)
									pOutDevice->EnableFadeIn(true);
								else
									pOutDevice->EnableFadeOut(true);
							}
						}
					}
				}
			}
		}
	}
}

void ContentProcessor::ChangeEffect(int iStreamIndex,int iEffectIndex,int iLevel, int* iParam)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();

#ifdef _ENABLE_SINGLE_EFFECT
	manager->GetDisplayManager()->SetEffectIndex(iEffectIndex);
	manager->GetDisplayManager()->SetEffectParam(iLevel, iParam[0], iParam[1], iParam[2], iParam[3], iParam[4]);

	for (int i = 0; i < _TL_MAX_LAYER; i++)
	{
		OutDevice* pOutDevice = manager->GetOutDevice(i);
		if (pOutDevice)
		{
			pOutDevice->SetEffectIndex(iEffectIndex);
			pOutDevice->SetEffectLevel(iLevel);
			pOutDevice->SetEffectParam1(iParam[0]);
			pOutDevice->SetEffectParam2(iParam[1]);
			pOutDevice->SetEffectParam3(iParam[2]);
			pOutDevice->SetEffectParam4(iParam[3]);
			pOutDevice->SetEffectParam5(iParam[4]);
		}
	}
#endif
	OutDevice* pOutDevice = manager->GetOutDevice(iStreamIndex);
	if (pOutDevice)
	{
		pOutDevice->SetEffectIndex(iEffectIndex);
		pOutDevice->SetEffectLevel(iLevel);
		pOutDevice->SetEffectParam1(iParam[0]);
		pOutDevice->SetEffectParam2(iParam[1]);
		pOutDevice->SetEffectParam3(iParam[2]);
		pOutDevice->SetEffectParam4(iParam[3]);
		pOutDevice->SetEffectParam5(iParam[4]);
	}
}

void ContentProcessor::ResetEffect(int iStreamIndex)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();

#ifdef _ENABLE_SINGLE_EFFECT
	manager->GetDisplayManager()->SetEffectIndex(0);
	manager->GetDisplayManager()->SetEffectParam(0, 50, 50, 50, 50, 50);

	for (int i = 0; i < _TL_MAX_LAYER; i++)
	{
		OutDevice* pOutDevice = manager->GetOutDevice(i);
		if (pOutDevice)
		{
			pOutDevice->SetEffectIndex(0);
			pOutDevice->SetEffectLevel(0);
		}
	}

#else
	OutDevice* pOutDevice = manager->GetOutDevice(iStreamIndex);
	if (pOutDevice)
	{
		pOutDevice->SetEffectIndex(0);	
		pOutDevice->SetEffectLevel(0);
	}
#endif
}

void ContentProcessor::Write2DecryptFileList(char* szFileName)
{
	bool bFound = false;
	wchar_t wszKeyName[256];
	wchar_t wszData[512];

	SIniFileW* pSIniFileW_R = new SIniFileW;

	pSIniFileW_R->Open(wszIniFileName, true, false);

	wchar_t* wszTotal = pSIniFileW_R->GetItemValue(L"FileList", L"TotalOfFiles", L"0");
	int iTotalOfFiles = _wtoi(wszTotal);
	for (int j = 0; j < iTotalOfFiles; j++)
	{
		wsprintfW(wszKeyName, L"File%d", j);

		wchar_t* wszCurFileName = pSIniFileW_R->GetItemValue(L"FileList", wszKeyName, L"");
		char* szCurFileName = WCharToChar(wszCurFileName);
		if (strcmp(szCurFileName, szFileName) == 0)
		{
			bFound = true;
			break;
		}
	}


	if (!bFound)
	{
		m_pSIniFileW->Open(wszIniFileName, true, true);

		wsprintfW(wszData, L"%d", iTotalOfFiles + 1);
		m_pSIniFileW->SetItemData(L"FileList", L"TotalOfFiles", wszData);

		for (int j = 0; j < iTotalOfFiles; j++)
		{
			wsprintfW(wszKeyName, L"File%d", j);

			wchar_t* wszCurFileName = pSIniFileW_R->GetItemValue(L"FileList", wszKeyName, L"");
			m_pSIniFileW->SetItemData(L"FileList", wszKeyName, wszCurFileName);
		}

		wsprintfW(wszKeyName, L"File%d", iTotalOfFiles);
		wcscpy(wszData, ANSIToUnicode(szFileName));
		m_pSIniFileW->SetItemData(L"FileList", wszKeyName, wszData);

		m_pSIniFileW->Write();
		m_pSIniFileW->Close();
	}
	pSIniFileW_R->Close();
	delete pSIniFileW_R;
}

bool ContentProcessor::CheckFileEncryptStatus(wchar_t* wszFileName)
{
	auto manager = ServiceProvider::Instance()->GetTimelineService()->GetTimelineManager();
	auto track_content_manager = manager->GetTrackContentManager();
	IOSourceCtrlDll* pIOSourceCtrlDll = track_content_manager->GetIOSourceCtrlDll();

	MediaFileInfo* pCurItem = pIOSourceCtrlDll->GetMediaFileInfo(wszFileName);
	if (pCurItem)
		return pCurItem->bIsEncrypted;
	return false;
}
