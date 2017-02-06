
// MyServer.cpp : ���� ���α׷��� ���� Ŭ���� ������ �����մϴ�.
//

#include "stdafx.h"
#include "MyServer.h"
#include "MyServerDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyServerApp

BEGIN_MESSAGE_MAP(CMyServerApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CMyServerApp ����

CMyServerApp::CMyServerApp()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	// InitInstance�� ��� �߿��� �ʱ�ȭ �۾��� ��ġ�մϴ�.
}


// ������ CMyServerApp ��ü�Դϴ�.

CMyServerApp theApp;


// CMyServerApp �ʱ�ȭ

BOOL CMyServerApp::InitInstance()
{
	CWinApp::InitInstance();

	WSADATA temp;
	WSAStartup(0x0202, &temp);  //��ȭ���� ������ ���� ����Ѵ�.

	CMyServerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	WSACleanup(); //��ȭ���� ������ ���� �ʱ�ȭ, �� ���̻� ������� �ʴ´�.

	return FALSE;
}

