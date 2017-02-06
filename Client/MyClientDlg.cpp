
// MyClientDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "MyClient.h"
#include "MyClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyClientDlg 대화 상자



CMyClientDlg::CMyClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MYCLIENT_DIALOG, pParent), m_white_brush(RGB(255, 255, 255)), m_black_brush(RGB(0, 0, 0))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int y = 0; y < MAX_Y_COUNT; y++) {
		for (int x = 0; x < MAX_X_COUNT; x++) {
			m_dol_pos[y][x] = 0; //바둑 전체판에 0(빈좌표)을 셋팅
		}
	}

	mh_socket = INVALID_SOCKET;
	m_connected_flag = 0; // 0은 쓸수 없는 상태를 의미한다.
}

void CMyClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EVENT_LIST, m_event_list);
}

BEGIN_MESSAGE_MAP(CMyClientDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(28001, &CMyClientDlg::On28001)
	ON_BN_CLICKED(IDC_SEND_BTN, &CMyClientDlg::OnBnClickedSendBtn)
	ON_MESSAGE(28002, &CMyClientDlg::On28002)
	ON_BN_CLICKED(IDOK, &CMyClientDlg::OnBnClickedOk)
	ON_WM_LBUTTONDOWN()
	ON_BN_CLICKED(IDCANCEL, &CMyClientDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CMyClientDlg 메시지 처리기

BOOL CMyClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	mh_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in srv_addr;
	memset(&srv_addr, 0, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	//cmd ipconfig의 pv4주소
	srv_addr.sin_port = htons(15003); //주소와 포트가 서버와 다르면 붙지못한다.
	//클라이언트는 서비스하는게 아니라 접속만하면 되기 때문에 bind함수를 쓰지 않아도 된다.

	WSAAsyncSelect(mh_socket, m_hWnd, 28001, FD_CONNECT);
	//접속에 성공하든 실패하든 FD_CONNECT메세지가 발생한다.
	
	m_connected_flag = 1;
	//flag가 0이면 Invalid_socket의 소켓자체가 없이 서버에 접속을 하지 않은 상태
	//1이면 소켓에 Invalid_socket가 아닌 다른값이 들어가 생성되었으나 접속을 하기전 시도중인 상태

	connect(mh_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));
	//서버가 준비안돼 있으면 28초간 응답없음 상태에 빠질수 있기 때문에 비동기를 걸어준다.
	//비동기식이 없다면 접속에 실패하면 응답없음이 되기 때문이다.
	
	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMyClientDlg::OnPaint()
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
		CPaintDC dc(this);

		for (int y = 1; y < MAX_Y_COUNT; y++) {  // 바둑판 그려주는 코드
			for (int x = 1; x < MAX_X_COUNT; x++) {
				dc.Rectangle(x*LINE_INTERVAL, y*LINE_INTERVAL,
					x*LINE_INTERVAL + LINE_INTERVAL + 1, y*LINE_INTERVAL + LINE_INTERVAL + 1);
			}
		}
		CBrush *p_old_brush;

		for (int y = 1; y <= MAX_Y_COUNT; y++) {  //지워진 바둑돌을 다시 그려주는 코드
			for (int x = 1; x <= MAX_X_COUNT; x++) {
				if (m_dol_pos[y - 1][x - 1]) {
					if (m_dol_pos[y - 1][x - 1] == 1) {
						p_old_brush = dc.SelectObject(&m_black_brush);
					}
					else {
						p_old_brush = dc.SelectObject(&m_white_brush);
					}
					dc.Ellipse(x*LINE_INTERVAL - LINE_INTERVAL / 2, y*LINE_INTERVAL - LINE_INTERVAL / 2,
						x*LINE_INTERVAL + LINE_INTERVAL + 1 - LINE_INTERVAL / 2,
						y*LINE_INTERVAL + LINE_INTERVAL + 1 - LINE_INTERVAL / 2);

					dc.SelectObject(p_old_brush);
				}
			}
		}
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMyClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



afx_msg LRESULT CMyClientDlg::On28001(WPARAM wParam, LPARAM lParam)
//커넥트를 시도한 소켓(mh_socket)의 핸들이 wParam에 전달
//클라이언트는 소켓이 하나이기 때문에 wParam을 쓸필요 없다.
//lParam은 어떤이벤트인지는 FD_CONNECT와 에러가 발생했는지 들어간다.
//이벤트는 하나 뿐이라 에러발생했는지만 확인한다.
{
	if (WSAGETSELECTERROR(lParam)) {
		AddEvent("서버연결에 실패하였습니다.");
		m_connected_flag = 0;
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
	}

	else {
		AddEvent("서버연결에 성공하였습니다.");
	//이 시점에 소켓이 서버에 붙은상태이므로 이때부터 소켓에 데이터를 쏠 수 있다.

		m_connected_flag = 2;
		//서버와 연결하여 데이터 통신이 가능한 상태
		//flag값이 2인지 확인하고 데이터를 전송하도록 할 것 이다.

		WSAAsyncSelect(mh_socket, m_hWnd, 28002, FD_READ | FD_CLOSE);
		//소켓이 하나라서 wParam == mh_socket이므로 wParam을 쓸필요가 없다.
	}
	return 0;
}

void CMyClientDlg::AddEvent(CString a_string)
//넘어온 문자열을 리스트 박스에 추가할 함수
//사용자가 문자열을 출력할 함수
{
	while (m_event_list.GetCount() > 1000) {
		m_event_list.DeleteString(0);
	}//호출한 시점에 1000개 이상있어도 함수가 진행되도록 한다.
	 //if문이면 문제생김

	int index = m_event_list.InsertString(-1, a_string);
	m_event_list.SetCurSel(index);
}

void CMyClientDlg::SendFrameData(SOCKET ah_socket,unsigned char a_msg_id, const char *ap_data, unsigned short int a_data_size)
//어떤 소켓으로,어떤 메세지인지, 어떤데이터를, 얼마만큼 보낼지가 매개변수로 전달된다.
{
	int send_data_size = a_data_size + 4;
	char *p_send_data = new char[send_data_size];//여기까지가 전송할 크기를 만들어냄
	
	*p_send_data = 27; //유효한 메세지인지 확인시킴 //첫번째 헤더
	*(p_send_data + 1) = a_msg_id; // 두번째 1바이트에 msg_id
	*(unsigned short int*)(p_send_data + 2) = a_data_size; // 포인터의 변위를 2바이트로 캐스팅한다.
	memcpy(p_send_data + 4, ap_data, a_data_size);
	//ap_data를 a_data_size의 크기만큼 복사해서 p_send_data[4]에 복사하겠다.
	send(mh_socket, p_send_data, send_data_size, 0);


	delete[] p_send_data; 
}

void CMyClientDlg::OnBnClickedSendBtn()
//데이터 전송 함수
{
	if (m_connected_flag == 2) {
		CString text, str;
		GetDlgItemText(IDC_CHAT_EDIT, text);
		//에디트박스에서 문장을 가져온다
		str = client_name;
		str += " : ";
		str += text;

		SendFrameData(mh_socket, 1, str, str.GetLength()+1);
		//mh_sockey으로 매세지아이디 1을 보내고 str의 내용을 보낼것이다, str의 사이즈는 str.GetLength인데
		//백슬래쉬를 뺀 사이즈를 보내므로 1을 더해준다
		SetDlgItemText(IDC_CHAT_EDIT, ""); //엔터치는 순간 데이터를 보내고 빈칸으로 초기화한다.

	}//서버로 데이터를 쏴준다
}


afx_msg LRESULT CMyClientDlg::On28002(WPARAM wParam, LPARAM lParam)
//READ 메세지를 처리할 함수 RECV
//데이터를 받을때 사용됨
{
	if (WSAGETSELECTEVENT(lParam) == FD_READ) {
		WSAAsyncSelect(mh_socket, m_hWnd, 28002, FD_CLOSE);
		
		unsigned char check, msg_id;
		unsigned short int body_size;

		recv(mh_socket, (char*)&check, 1, 0); 
		//첫번째 바이트가 27의값과 같은지 확인한다.
		CString str;

		if (check == 27) {
			recv(mh_socket, (char*)&msg_id, 1, 0);
			recv(mh_socket, (char*)&body_size, 2, 0);
			if (body_size > 0) {
				char *p_body_data = new char[body_size];
				recv(mh_socket, p_body_data, body_size, 0);

				if (msg_id == 0) {
					//client 정보
					client_name = "Client";
					client_name += p_body_data;
					m_step_count = _ttoi(p_body_data);
				}

				else if (msg_id == 1) {
					//채팅 데이터를 recv
					AddEvent(p_body_data);
				}

				else if (msg_id == 2) {
					CClientDC dc(this);

					str = p_body_data;
					int pos = _ttoi(str);	//CString to integer

					int color = pos / 10000;
					int y = (pos - color*10000) / 100;
					int x = (pos - color*10000) % 100;
					
					CBrush *p_old_brush;
					m_dol_pos[y - 1][x - 1] = color;
					if (color == 1)
						p_old_brush = dc.SelectObject(&m_black_brush);
					else
						p_old_brush = dc.SelectObject(&m_white_brush);

					x = x*LINE_INTERVAL;
					y = y*LINE_INTERVAL;
					dc.Ellipse(x - LINE_INTERVAL / 2, y - LINE_INTERVAL / 2, 
								x + LINE_INTERVAL / 2, y + LINE_INTERVAL / 2);
					dc.SelectObject(p_old_brush);
					
					m_step_count++;
					printf("에러없음\n");
				}

				delete[] p_body_data;
			}// 데이터를 받는 코드

			WSAAsyncSelect(mh_socket, m_hWnd, 28002, FD_READ | FD_CLOSE);
		}
	}
	else
	{
		AddEvent("서버접속이 끊어졌습니다.");
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
		m_connected_flag = 0;
	}
	return 0;
}


void CMyClientDlg::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
    //	CDialogEx::OnOK(); //ok버튼을 클릭하면 종료된다는 의미로 닫으면 종료되지 않는다.
}


void CMyClientDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CClientDC dc(this);
	if (m_step_count % 2 == 0) {
		CString str;

		int x = point.x / LINE_INTERVAL;
		int y = point.y / LINE_INTERVAL;

		if (point.x % LINE_INTERVAL > LINE_INTERVAL / 2) x++;
		if (point.y % LINE_INTERVAL > LINE_INTERVAL / 2) y++;

		int pos = 100 * y + x;

		if (x > 0 && y > 0 && x <= MAX_X_COUNT &&y <= MAX_Y_COUNT) {
			if (m_dol_pos[y - 1][x - 1] == 0) {  //해당 좌표에 돌이 없다면 if문을 실행한다.

				printf("client\n x = %d, y = %d\n", x, y);
				printf("pos = %d", pos);

				str.Format(_T("%d"), pos); //integer to CString

				SendFrameData(mh_socket, 2, str, str.GetLength() + 1);
			}
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CMyClientDlg::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}
