// �U�C ifdef �϶��O�إߥ����H��U�q DLL �ץX���зǤ覡�C
// �o�� DLL �����Ҧ��ɮ׳��O�ϥΩR�O�C���ҩw�q RTSPSTREAMCLIENT_EXPORTS �Ÿ��sĶ���C
// ����ϥγo�� DLL ���M�׳������w�q�o�ӲŸ��C
// �o�ˤ@�ӡA��l�{���ɤ��]�t�o�ɮת������L�M��
// �|�N RTSPSTREAMCLIENT_API �禡�����q DLL �פJ���A�ӳo�� DLL �h�|�N�o�ǲŸ�����
// �ץX���C
#ifdef RTSPSTREAMCLIENT_EXPORTS
#define RTSPSTREAMCLIENT_API __declspec(dllexport)
#else
#define RTSPSTREAMCLIENT_API __declspec(dllimport)
#endif

/*
// �o�����O�O�q RtspStreamClient.dll �ץX��
class RTSPSTREAMCLIENT_API CRtspStreamClient {
public:
	CRtspStreamClient(void);
	// TODO: �b���[�J�z����k�C
};

extern RTSPSTREAMCLIENT_API int nRtspStreamClient;

RTSPSTREAMCLIENT_API int fnRtspStreamClient(void);
*/