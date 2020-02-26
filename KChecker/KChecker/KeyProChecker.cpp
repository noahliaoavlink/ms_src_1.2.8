#include "stdafx.h"
#include "KeyProChecker.h"

#include "PlaneScene3D.h"

#ifdef _WIN64
#pragma comment(lib,"..\\..\\x64\\Debug\\3DWorld.lib")
#endif

#ifdef _WIN64
#pragma comment(lib,"..\\..\\lib\\HASP\\libhasp_windows_x64_34720.lib")
#else
#pragma comment(lib,"..\\lib\\HASP\\libhasp_windows_34720.lib")
#endif

unsigned char vendor_code[] =
"xDXLpqde16FbWnjk2jabaGaB4gE9pKtCBh5cMdmy2g8FTF6jyP0/7V6NC5ETk+7Oaw3C78YQiRFmXayY"
"cTWRh6sLq5idglcFPfbSbwGia1/98+DuK97KEAHR3ulAodJbOT4BFdBr7ewVDdPTGa2fMTcBaE28IgjP"
"9fIIz3ptTdsKXd1YeDE/N4fmclvOpHMKB/1ZDoCHoCvOcTaSh9TlMU/L7eL5U3qu7PwTqWvJXmRrGRI8"
"GvC+1WD46va87mpcXH+0gCgCuVfLN/Dz2XSr/P5xSqEjT0VIKwVGnG29QxdFJHXpaI95vGssCHgMD3bs"
"GhsfSwJ90nwOHbbHpN7M1iN8YyBlcxHUTl54/QMTcPEYTfJ3mwDOsy7oUdEinERh1X7HqIogB/WhUJTP"
"LnEiXvRCY8x57TnRXRqu2mam15tA2rpHDHlyD8Dg8+5tJWJ0bazipvfm0U7uJ8f7mThlRnsbM7sH3QTY"
"39Qv9nd4fpz47BBYt219O6N4hyrkRt9kvoNCUobjaH2oIXsMDKfAbLZSgOvfi5enT2gowyqgNdoQQN7q"
"qa3Jdd6UzssXElxLD7krPQcKQYTvGGK/JK8lysblSIZdHOK9x0VBiBxGio4r3CV1FfVSKhX0WcZuRA1h"
"SRtumJe2N+oGJJ01a/ngKQD37x3bUnPyDORQMlIbJpTc8fiq/770hOlwLflEeE/4+DoPkl5iZ6rmskM3"
"8Hub1KX510Q2CMZjxe83iRzkszC42a1Xe6qCnqQtn2bM1rtVU18V5Hdmou9P6Jcy+9WRPBBQUioKQWLk"
"5CKnDnMRuDbX7FW4yAF0E2ivtKdiktVCKgPzk9ymJ7fOMlz0mdcpoCuk4s2NCJ3uZprTqaZDFop/Rgdi"
"PeSKKuVDvX7J6KV9UdDQNtXma4IzdotKh9LN7U8ApToT8GeXGlcpanSTkMX7WZQxyhJlP4mQvbDKcQH7"
"SSvOo8AOgSY+hE6n5xIrgg==";


const hasp_feature_t feature = 1;
hasp_handle_t handle = HASP_INVALID_HANDLE_VALUE;
hasp_status_t status;

KeyProChecker::KeyProChecker()
{
}

KeyProChecker::~KeyProChecker()
{
}

void KeyProChecker::Check()
{
	COPYDATASTRUCT copy_data;

	status = hasp_login(feature, vendor_code, &handle);

	HWND hMCReciever = FindWindowA("MidiCtrl_Reciever", NULL);

	if (status != HASP_STATUS_OK)
	{
		if (hMCReciever)
		{
			copy_data.dwData = KPCC_RET_FAILED;
			copy_data.cbData = 0;
			copy_data.lpData = 0;

			SendMessage(hMCReciever, WM_COPYDATA, 0, (LPARAM)&copy_data);
		}
	}
	else
	{
		if (hMCReciever)
		{
			copy_data.dwData = KPCC_RET_OK;
			copy_data.cbData = 0;
			copy_data.lpData = 0;

			SendMessage(hMCReciever, WM_COPYDATA, 0, (LPARAM)&copy_data);
		}
	}
	Sleep(1000);
}