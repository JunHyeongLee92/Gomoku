
// MyServerDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"

#define MAX_COUNT 2  //����ڸ� 2������� �ްڴ�

#define MAX_X_COUNT 11
#define MAX_Y_COUNT 11  //�ٱ������� 11���̴� �簢���� 100�� �׸���.


struct ClientSocket {
	SOCKET h_socket;
	char ip_address[16]; // ip�ּ� �ִ� 255.255.255.255
};

// CMyServerDlg ��ȭ ����
class CMyServerDlg : public CDialogEx
{
// �����Դϴ�.
private:
	SOCKET mh_socket;
	ClientSocket m_client_list[MAX_COUNT]; 

	//Ŭ���� ���� ��ü�����ڸ� ���� ���Ѵ�.
	int color, m_step_count = 0;

public:
	CMyServerDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	void AddEvent(CString a_string);
	void SendFrameData(SOCKET ah_socket, unsigned char a_msg_id, const char *ap_data, unsigned short int a_data_size);
	static UINT AcceptFunc(LPVOID pParam);
	static UINT RecvFunc(LPVOID pParam);


	
// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
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