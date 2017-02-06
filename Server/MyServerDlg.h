
// MyServerDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

#define MAX_COUNT 2  //사용자를 2명까지만 받겠다

#define MAX_X_COUNT 11
#define MAX_Y_COUNT 11  //줄기준으로 11줄이다 사각형은 100개 그린다.


struct ClientSocket {
	SOCKET h_socket;
	char ip_address[16]; // ip주소 최대 255.255.255.255
};

// CMyServerDlg 대화 상자
class CMyServerDlg : public CDialogEx
{
// 생성입니다.
private:
	SOCKET mh_socket;
	ClientSocket m_client_list[MAX_COUNT]; 

	//클래스 내에 객체생성자를 선언 못한다.
	int color, m_step_count = 0;

public:
	CMyServerDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	void AddEvent(CString a_string);
	void SendFrameData(SOCKET ah_socket, unsigned char a_msg_id, const char *ap_data, unsigned short int a_data_size);
	static UINT AcceptFunc(LPVOID pParam);
	static UINT RecvFunc(LPVOID pParam);


	
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT On25001(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On25002(WPARAM wParam, LPARAM lParam);
private:
	CListBox m_event_list;
public:
//	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

class CRecvParam
{
public:
	CRecvParam(CMyServerDlg* pDlg, SOCKET ClientSocket)
		:m_pDlg(pDlg), m_ClientSocket(ClientSocket)
	{

	}
	~CRecvParam()
	{
		closesocket(m_ClientSocket);
	}
	CMyServerDlg* m_pDlg;
	SOCKET m_ClientSocket;
};