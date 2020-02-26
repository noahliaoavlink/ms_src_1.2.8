#include "stdafx.h"
#include "TL_Out_Device.h"

OutDeviceManager::OutDeviceManager()
{
	for (int i = 0; i < _TL_MAX_LAYER; i++)
	{
		m_pLayerOutDevice[i] = 0;
		m_iLayerPBStatus[i] = PBC_NONE;
	}
	m_pMainStream = 0;
	
	m_iMainStreamPBStatus = PBC_NONE;
}

OutDeviceManager::~OutDeviceManager()
{
}

void OutDeviceManager::SetMainStreamOutDevice(OutDevice* m_pObj)
{
	m_pMainStream = m_pObj;
}

void OutDeviceManager::SetLayerOutDevice(int iIndex, OutDevice* m_pObj)
{
	if(_TL_MAX_LAYER > iIndex)
		m_pLayerOutDevice[iIndex] = m_pObj;
}

OutDevice* OutDeviceManager::GetMainStreamOutDevice()
{
	return m_pMainStream;
}

OutDevice* OutDeviceManager::GetLayerOutDevice(int iIndex)
{
	if (_TL_MAX_LAYER > iIndex)
		return m_pLayerOutDevice[iIndex];
	return 0;
}

void OutDeviceManager::SetLayerPBStatus(int iIndex, int iCmd)
{
	if (_TL_MAX_LAYER > iIndex)
		m_iLayerPBStatus[iIndex] = iCmd;
}

int OutDeviceManager::GetLayerPBStatus(int iIndex)
{
	if (_TL_MAX_LAYER > iIndex)
		return m_iLayerPBStatus[iIndex];
	return 0;
}

void OutDeviceManager::SetMainStreamPBStatus(int iCmd)
{
	m_iMainStreamPBStatus = iCmd;
}

int OutDeviceManager::GetMainStreamPBStatus()
{
	return m_iMainStreamPBStatus;
}

TL_Out_Device::TL_Out_Device()
{
	m_pOutDeviceManager = new OutDeviceManager;
}

TL_Out_Device::~TL_Out_Device()
{
	if (m_pOutDeviceManager)
		delete m_pOutDeviceManager;
}

void TL_Out_Device::SetOutDevice(int iType, OutDevice* pObj)
{
	switch (iType)
	{
		case ST_MAIN:
			m_pOutDeviceManager->SetMainStreamOutDevice(pObj);
			break;
		case ST_LAYER1:
		case ST_LAYER2:
		case ST_LAYER3:
			m_pOutDeviceManager->SetLayerOutDevice(iType - 1,pObj);
			break;
		case ST_STREAM0:
		case ST_STREAM1:
		case ST_STREAM2:
		case ST_STREAM3:
		case ST_STREAM4:
		case ST_STREAM5:
		case ST_STREAM6:
		case ST_STREAM7:
		case ST_STREAM8:
		case ST_STREAM9:
			m_pOutDeviceManager->SetLayerOutDevice(iType - 1, pObj);
			break;
	}
}

OutDevice* TL_Out_Device::GetOutDevice(int iType)
{
	switch (iType)
	{
		case ST_MAIN:
			return m_pOutDeviceManager->GetMainStreamOutDevice();
			break;
		case ST_LAYER1:
		case ST_LAYER2:
		case ST_LAYER3:
			return m_pOutDeviceManager->GetLayerOutDevice(iType - 1);
			break;
		case ST_STREAM0:
		case ST_STREAM1:
		case ST_STREAM2:
		case ST_STREAM3:
		case ST_STREAM4:
		case ST_STREAM5:
		case ST_STREAM6:
		case ST_STREAM7:
		case ST_STREAM8:
		case ST_STREAM9:
			return m_pOutDeviceManager->GetLayerOutDevice(iType - 1);
			break;
	}

	return 0;
}

void TL_Out_Device::SetLayerPBStatus(int iIndex, int iCmd)
{
	m_pOutDeviceManager->SetLayerPBStatus(iIndex,iCmd);
}

int TL_Out_Device::GetLayerPBStatus(int iIndex)
{
	return m_pOutDeviceManager->GetLayerPBStatus(iIndex);
}

void TL_Out_Device::SetMainStreamPBStatus(int iCmd)
{
	m_pOutDeviceManager->SetMainStreamPBStatus(iCmd);
}

int TL_Out_Device::GetMainStreamPBStatus()
{
	return m_pOutDeviceManager->GetMainStreamPBStatus();
}
