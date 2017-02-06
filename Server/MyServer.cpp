
// MyServer.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
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


// CMyServerApp 생성

CMyServerApp::CMyServerApp()
{
	// TODO: 여기에 생성 코드를 추가합니다.
	// InitInstance에 모든 중요한 초기화 작업을 배치합니다.
}


// 유일한 CMyServerApp 개체입니다.

CMyServerApp theApp;


// CMyServerApp 초기화

BOOL CMyServerApp::InitInstance()
{
	CWinApp::InitInstance();

	WSADATA temp;
	WSAStartup(0x0202, &temp);  //대화상자 열리면 소켓 사용한다.

	CMyServerDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	WSACleanup(); //대화상자 닫히면 소켓 초기화, 즉 더이상 사용하지 않는다.

	return FALSE;
}

