// 下列 ifdef 區塊是建立巨集以協助從 DLL 匯出的標準方式。
// 這個 DLL 中的所有檔案都是使用命令列中所定義 RTSPSTREAMCLIENT_EXPORTS 符號編譯的。
// 任何使用這個 DLL 的專案都不應定義這個符號。
// 這樣一來，原始程式檔中包含這檔案的任何其他專案
// 會將 RTSPSTREAMCLIENT_API 函式視為從 DLL 匯入的，而這個 DLL 則會將這些符號視為
// 匯出的。
#ifdef RTSPSTREAMCLIENT_EXPORTS
#define RTSPSTREAMCLIENT_API __declspec(dllexport)
#else
#define RTSPSTREAMCLIENT_API __declspec(dllimport)
#endif

/*
// 這個類別是從 RtspStreamClient.dll 匯出的
class RTSPSTREAMCLIENT_API CRtspStreamClient {
public:
	CRtspStreamClient(void);
	// TODO: 在此加入您的方法。
};

extern RTSPSTREAMCLIENT_API int nRtspStreamClient;

RTSPSTREAMCLIENT_API int fnRtspStreamClient(void);
*/