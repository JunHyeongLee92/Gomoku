
// MyClient.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "MyClient.h"
#include "MyClientDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyClientApp

BEGIN_MESSAGE_MAP(CMyClientApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMyClientApp ����

CMyClientApp::CMyClientApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CMyClientApp ��ü�Դϴ�.

CMyClientApp theApp;


// CMyClientApp �ʱ�ȭ

BOOL CMyClientApp::InitInstance()
{
	CWinApp::InitInstance();
	WSADATA temp;
	WSAStartup(0x0202, &temp);

	CMyClientDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	WSACleanup();
	
	return FALSE;
}