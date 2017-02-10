
// MyServerDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MyServer.h"
#include "MyServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection g_cs;

// CMyServerDlg 대화 상자


CMyServerDlg::CMyServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MYSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i = 0; i < MAX_COUNT; i++) {
		m_client_list[i].h_socket = INVALID_SOCKET; 
		//어떤 값이 들어 왔을때 invalid_socket가 들어있는 값은 빈방이고 다른값이 있는방은 건너뛰도록 한다.
	}
}

void CMyServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVENT_LIST, m_event_list);
}

BEGIN_MESSAGE_MAP(CMyServerDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(25001, &CMyServerDlg::On25001)
	ON_MESSAGE(25002, &CMyServerDlg::On25002)
//	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


// CMyServerDlg 메시지 처리기

BOOL CMyServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	mh_socket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET = INET방식으로 통신을 하겠다.
		//0으로 해놓으면 통신타입에 맞는 프로토콜을 알아서 찾아준다
		//부호없는 32비트 unsigned int를 이름만 바꾼것 = socket
	sockaddr_in srv_addr; //srv_addr를 개통하기위해 제출할 서류라고 생각하면된다.
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = INADDR_ANY; //서비스할 address
	srv_addr.sin_port = htons(15003); //내프로그램안에서 어떤 컴퓨터와 통신할지를 정한다
	//port는 중복되선 안된다. 먼저선점한자가 임자
	//포트번호는 15000번이상 쓴다

	bind(mh_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));
	listen(mh_socket, 1); 
	//1은 동시에 call받을 수 있는 숫자, 즉 한번에 한명에게만 수신 받을 수 있다.

	AfxBeginThread(AcceptFunc, this);

	AddEvent("서버 서비스를 시작합니다.");

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMyServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMyServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//클라이언트를 계속해서 받아주는 스레드에 사용할 함수
UINT CMyServerDlg::AcceptFunc(LPVOID pParam)
{
	CMyServerDlg *pDlg = (CMyServerDlg*)pParam;


	while (true)
	{
		//이곳에 루프를 반복하며 수행할 동작을 넣습니다.
		SOCKADDR_IN addr;
		memset(&addr, 0, sizeof(addr));
		int iAddrLen = sizeof(addr), i;

		SOCKET ClientSocket = accept(pDlg->mh_socket, (sockaddr*)&addr, &iAddrLen);
		if (ClientSocket == INVALID_SOCKET)
			break;

		//스레드 동기화 임계영역
	
		g_cs.Lock();
		for (i = 0; i < MAX_COUNT; i++) {
			if (pDlg->m_client_list[i].h_socket == INVALID_SOCKET) break;
		}
		if (i < MAX_COUNT) { //빈방이 있을때만 accept를 한다.
			pDlg->m_client_list[i].h_socket = ClientSocket;
			//sockaddr_in_size는 접속자의 로그를 남겨준다		
			strcpy(pDlg->m_client_list[i].ip_address, inet_ntoa(addr.sin_addr));
			//ip주소 저장

			CString str = "새로운 사용자가 접속했습니다 : ";
			str += pDlg->m_client_list[i].ip_address; //새로운 사용자가 접속하면 ip를 띄워준다.

			pDlg->AddEvent(str);

			pDlg->SendFrameData(pDlg->m_client_list[i].h_socket, 1, str, str.GetLength() + 1);

			str.Format(_T("%d"), i);	//client 번호 전송
			pDlg->AddEvent(str);
			pDlg->SendFrameData(pDlg->m_client_list[i].h_socket, 0, str, str.GetLength() + 1);

			AfxBeginThread(RecvFunc, new CRecvParam(pDlg, ClientSocket));

		}
		else {
			closesocket(ClientSocket);
		}
		g_cs.Unlock();
	}
	return 0;
}

//클라이언트로 부터 메세지를 받아주는 스레드에 사용하는 함수
UINT CMyServerDlg::RecvFunc(LPVOID pParam)
{
	CRecvParam *p = (CRecvParam*)pParam;
	CMyServerDlg *pDlg = p->m_pDlg;
	SOCKET ClientSocket = p->m_ClientSocket;;

	while (true) {

		unsigned char check, msg_id;//message_id
		unsigned short int body_size;

		recv(ClientSocket, (char*)&check, 1, 0); //헤더 1바이트씩 읽음 
													 //헤더 유효성 체크하는 부분
		if (check == 27) {
			recv(ClientSocket, (char*)&msg_id, 1, 0); //메세지 종류 부분을 읽음
			recv(ClientSocket, (char*)&body_size, 2, 0); //body를 읽을 수 있는 사이즈 부분을 읽음
			if (body_size > 0) {
				char *p_body_data = new char[body_size]; //바디사이즈 읽어와서 동적할당한다.
				recv(ClientSocket, p_body_data, body_size, 0);
				CString str;
				if (msg_id == 1) {
					str = p_body_data;
					pDlg->AddEvent(str);
					for (int i = 0; i < MAX_COUNT; i++) {
						if (pDlg->m_client_list[i].h_socket != INVALID_SOCKET) {
							pDlg->SendFrameData(pDlg->m_client_list[i].h_socket, 1, str, str.GetLength() + 1);
							//자기한테 붙어있는 모든 클라이언트에게 받은 데이터를 전달해준다.
						}
					}
				}
				else if (msg_id == 2) {
					//바둑돌 좌표 데이터 recv

					str = p_body_data;
					int pos = _ttoi(str);	//CString to integer
					
					if (pDlg->m_step_count % 2)
						pDlg->color = 2;
					else
						pDlg->color = 1;
					pos += pDlg->color * 10000;	//좌표값에 바둑돌색깔값을 더함
					
					str.Format(_T("%d"), pos);
					pDlg->m_step_count++;
					for (int i = 0; i < MAX_COUNT; i++) {
						if (pDlg->m_client_list[i].h_socket != INVALID_SOCKET) {
							//자기한테 붙어있는 모든 클라이언트에게 받은 데이터를 전달해준다.
							pDlg->SendFrameData(pDlg->m_client_list[i].h_socket, 2, str, str.GetLength() + 1);
							// 바둑돌 좌표값 전송
						}
					}
				}
				delete[] p_body_data; //읽은 뒤 동적할당 해제해준다.
			}
		}	
	}
	delete p;
	return 0;
}


void CMyServerDlg::AddEvent(CString a_string)
{
	while (m_event_list.GetCount() > 1000) {
		m_event_list.DeleteString(0);
	}//호출한 시점에 1000개 이상있어도 함수가 진행되도록 한다.
	 //if문이면 문제생김

	int index = m_event_list.InsertString(-1, a_string);
	m_event_list.SetCurSel(index);
}

//서버와 데이터를 주고받을때 프레임화 시켜주는 함수
void CMyServerDlg::SendFrameData(SOCKET ah_socket, unsigned char a_msg_id, const char *ap_data, unsigned short int a_data_size)
//어떤 소켓으로,어떤 메세지인지, 어떤데이터를, 얼마만큼 보낼지가 매개변수로 전달된다.
{
	int send_data_size = a_data_size + 4;
	char *p_send_data = new char[send_data_size];//여기까지가 전송할 크기를 만들어냄

	*p_send_data = 27; //유효한 메세지인지 확인시킴 //첫번째 헤더
	*(p_send_data + 1) = a_msg_id; // 두번째 1바이트에 msg_id
	*(unsigned short int*)(p_send_data + 2) = a_data_size; // 포인터의 변위를 2바이트로 캐스팅한다.
	memcpy(p_send_data + 4, ap_data, a_data_size);
	//ap_data를 a_data_size의 크기만큼 복사해서 p_send_data[4]에 복사하겠다.
	send(ah_socket, p_send_data, send_data_size, 0);

	delete[] p_send_data;
}

//비동기식에서 사용하던 함수로 쓰레드에선 사용되지않는다
afx_msg LRESULT CMyServerDlg::On25001(WPARAM wParam, LPARAM lParam)
//이메세지가 발생하게된 원인의 소켓의 핸들이 wParam에 전달된다.
//lParam은 32비트인데 16비트는 어떤메세지인지 , 나머지 16비트는 에러가 발생했는지 들어있다.
{
	//SOCKET h_socket = (SOCKET)wParam;

	//sockaddr_in client_addr; //ip, 소켓 핸들을 기억해야한다.
	//int sockaddr_in_size = sizeof(sockaddr_in), i;
	//
	//for (i = 0; i < MAX_COUNT; i++) {
	//	if (m_client_list[i].h_socket == INVALID_SOCKET) break;
	//}
	//if (i < MAX_COUNT) { //빈방이 있을때만 accept를 한다.
	//	m_client_list[i].h_socket = accept(h_socket, (LPSOCKADDR)&client_addr, &sockaddr_in_size);
	//	//sockaddr_in_size는 접속자의 로그를 남겨준다		
	//	strcpy(m_client_list[i].ip_address, inet_ntoa(client_addr.sin_addr)); 
	//	//ip주소 저장

	//	CString str = "새로운 사용자가 접속했습니다 : ";
	//	str += m_client_list[i].ip_address; //새로운 사용자가 접속하면 ip를 띄워준다.
	//	
	//	AddEvent(str);

	//	SendFrameData(m_client_list[i].h_socket, 1, str, str.GetLength() + 1);

	//	str.Format(_T("%d"), i);	//client 번호 전송
	//	AddEvent(str);
	//	SendFrameData(m_client_list[i].h_socket, 0, str, str.GetLength() + 1);

	//	WSAAsyncSelect(m_client_list[i].h_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
	//	//비동기
	//	//현재윈도우에서 새로 accpet된 소켓에
	//	//상대편이 데이터를 주거나(fd_read) 나를 끊으면(fd_close) 메세지 25002를 줘라
	//}
	//else {
	//	closesocket(h_socket);
	//}

	return 0;
}

afx_msg LRESULT CMyServerDlg::On25002(WPARAM wParam, LPARAM lParam)
//lParam : 어떤 행위때문에 이 메세지가 발생했는가
//데이터를 READ할때 실행되는 함수
{
	//ClientSocket *p_user;

	//for (int i = 0; i < MAX_COUNT; i++) {
	//	if (m_client_list[i].h_socket == wParam) {
	//		/*if(m_client_list[i].h_socket != INVALID_SOCKET &&
	//		m_client_list[i].h_socket == wParam) */
	//		p_user = &m_client_list[i];
	//		/*int A[3]을 여러번 쓰면 매번  *(A+3)으로 실행돼서 덧셈연산이 들어가므로
	//		포인터를 하나 만들어 주소를 받아 쓰는게 이득이다.
	//		int *p;   p = &A[3];  */
	//		break;
	//	}
	//}
	//if (WSAGETSELECTEVENT(lParam) == FD_READ)//상대편이 나에게 데이터를 줬을때 25002메세지 발생
	//{
	//	WSAAsyncSelect(p_user->h_socket, m_hWnd, 25002, FD_CLOSE);
	//	//끊어 읽기를 할것 이기 때문에 25002가 무한발생하는 것을
	//	//막기 위해 읽는동안 읽음에 대한 비동기를 끊어준다.

	//	unsigned char check, msg_id;//message_id
	//	unsigned short int body_size;

	//	recv(p_user->h_socket, (char*)&check, 1, 0); //헤더 1바이트씩 읽음 
	//												 //헤더 유효성 체크하는 부분
	//	if (check == 27) {
	//		recv(p_user->h_socket, (char*)&msg_id, 1, 0); //메세지 종류 부분을 읽음
	//		recv(p_user->h_socket, (char*)&body_size, 2, 0); //body를 읽을 수 있는 사이즈 부분을 읽음

	//		if (body_size > 0) {
	//			char *p_body_data = new char[body_size]; //바디사이즈 읽어와서 동적할당한다.

	//			recv(p_user->h_socket, p_body_data, body_size, 0);

	//			CString str;

	//			if (msg_id == 1) {

	//				str = p_body_data;
	//				AddEvent(str);

	//				for (int i = 0; i < MAX_COUNT; i++) {
	//					if (m_client_list[i].h_socket != INVALID_SOCKET) {
	//						SendFrameData(m_client_list[i].h_socket, 1, str, str.GetLength() + 1);
	//						//자기한테 붙어있는 모든 클라이언트에게 받은 데이터를 전달해준다.
	//					}
	//				}

	//			}
	//			else if (msg_id == 2) {
	//				//바둑돌 좌표 데이터 recv

	//				str = p_body_data;
	//				int pos = _ttoi(str);	//CString to integer

	//				int y = pos / 100;
	//				int x = pos % 100;

	//				if (m_step_count % 2)
	//					color = 2;
	//				else
	//					color = 1;

	//				pos += color * 10000;	//좌표값에 바둑돌색깔값을 더함
	//				str.Format(_T("%d"), pos);
	//				m_step_count++;

	//				for (int i = 0; i < MAX_COUNT; i++) {
	//					if (m_client_list[i].h_socket != INVALID_SOCKET) {
	//						//자기한테 붙어있는 모든 클라이언트에게 받은 데이터를 전달해준다.
	//						SendFrameData(m_client_list[i].h_socket, 2, str, str.GetLength() + 1); // 바둑돌 좌표값 전송
	//					}
	//				}
	//			}
	//			delete[] p_body_data; //읽은 뒤 동적할당 해제해준다.
	//		}

	//		WSAAsyncSelect(p_user->h_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
	//		//작업을 다하고 다시 FD_READ에 대한 비동기를 걸어준다.
	//	}
	//}

	//else { //상대편이 나를 끊었을 때

	//	   //closesocket((SOCKET)wParam); //서버를 끊은 소켓의 주소를 불러와야한다
	//	closesocket(p_user->h_socket); //m_client_list[i].h_socket		
	//	p_user->h_socket = INVALID_SOCKET; //서버를 끊은 방에 INVALID_SOCKET값을 넣어준다.
	//}
	return 0;
}