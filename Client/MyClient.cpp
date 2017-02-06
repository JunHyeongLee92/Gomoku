
// MyClient.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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


// CMyClientApp 생성

CMyClientApp::CMyClientApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CMyClientApp 개체입니다.

CMyClientApp theApp;


// CMyClientApp 초기화

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