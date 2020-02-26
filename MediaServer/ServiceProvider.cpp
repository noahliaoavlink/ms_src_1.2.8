/********************************************************************\
Project Name: Timeline Prototype

File Name: ServiceProvider.cpp

Definition of following classes:
ServiceProvider

Copyright:
Media Server
(C) Copyright C&C TECHNIC TAIWAN CO., LTD.
All rights reserved.

Author:
Stone Chang, mail: stonechang.cctch@gmail.com

Other issue:
N/A

\********************************************************************/

// ==== Inlcude Microsoft MFC library ==== //
#include "stdafx.h"

// ==== Inlcude STL library ==== //

// ==== Include local library ==== //
#include "ServiceProvider.h"


using namespace Timeline;

/* ====================================================== *\
ServiceProvider
\* ====================================================== */
ServiceProvider ServiceProvider::m_this;

ServiceProvider::ServiceProvider()
{
	m_outputService = nullptr;
}

ServiceProvider::~ServiceProvider()
{
}

ServiceProvider* ServiceProvider::Instance()
{
	return &m_this;
}

OutputService* ServiceProvider::GetOutputService()
{
	return m_outputService;
}

void ServiceProvider::SetOutputService(OutputService* service)
{
	m_outputService = service;
}

TimelineService* ServiceProvider::GetTimelineService()
{
	return m_timelineService;
}

void ServiceProvider::SetTimelineService(TimelineService* service)
{
	m_timelineService = service;
}