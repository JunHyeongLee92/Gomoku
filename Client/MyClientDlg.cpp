
// MyClientDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MyClient.h"
#include "MyClientDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMyClientDlg ��ȭ ����



CMyClientDlg::CMyClientDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MYCLIENT_DIALOG, pParent), m_white_brush(RGB(255, 255, 255)), m_black_brush(RGB(0, 0, 0))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int y = 0; y < MAX_Y_COUNT; y++) {
		for (int x = 0; x < MAX_X_COUNT; x++) {
			m_dol_pos[y][x] = 0; //�ٵ� ��ü�ǿ� 0(����ǥ)�� ����
		}
	}

	mh_socket = INVALID_SOCKET;
	m_connected_flag = 0; // 0�� ���� ���� ���¸� �ǹ��Ѵ�.
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


// CMyClientDlg �޽��� ó����

BOOL CMyClientDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	mh_socket = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in srv_addr;
	memset(&srv_addr, 0, sizeof(struct sockaddr_in));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	//cmd ipconfig�� pv4�ּ�
	srv_addr.sin_port = htons(15003); //�ּҿ� ��Ʈ�� ������ �ٸ��� �������Ѵ�.
	//Ŭ���̾�Ʈ�� �����ϴ°� �ƴ϶� ���Ӹ��ϸ� �Ǳ� ������ bind�Լ��� ���� �ʾƵ� �ȴ�.

	WSAAsyncSelect(mh_socket, m_hWnd, 28001, FD_CONNECT);
	//���ӿ� �����ϵ� �����ϵ� FD_CONNECT�޼����� �߻��Ѵ�.
	
	m_connected_flag = 1;
	//flag�� 0�̸� Invalid_socket�� ������ü�� ���� ������ ������ ���� ���� ����
	//1�̸� ���Ͽ� Invalid_socket�� �ƴ� �ٸ����� �� �����Ǿ����� ������ �ϱ��� �õ����� ����

	connect(mh_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));
	//������ �غ�ȵ� ������ 28�ʰ� ������� ���¿� ������ �ֱ� ������ �񵿱⸦ �ɾ��ش�.
	//�񵿱���� ���ٸ� ���ӿ� �����ϸ� ��������� �Ǳ� �����̴�.
	
	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMyClientDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // �׸��⸦ ���� ����̽� ���ؽ�Ʈ�Դϴ�.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Ŭ���̾�Ʈ �簢������ �������� ����� ����ϴ�.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// �������� �׸��ϴ�.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CPaintDC dc(this);

		for (int y = 1; y < MAX_Y_COUNT; y++) {  // �ٵ��� �׷��ִ� �ڵ�
			for (int x = 1; x < MAX_X_COUNT; x++) {
				dc.Rectangle(x*LINE_INTERVAL, y*LINE_INTERVAL,
					x*LINE_INTERVAL + LINE_INTERVAL + 1, y*LINE_INTERVAL + LINE_INTERVAL + 1);
			}
		}
		CBrush *p_old_brush;

		for (int y = 1; y <= MAX_Y_COUNT; y++) {  //������ �ٵϵ��� �ٽ� �׷��ִ� �ڵ�
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

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMyClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



afx_msg LRESULT CMyClientDlg::On28001(WPARAM wParam, LPARAM lParam)
//Ŀ��Ʈ�� �õ��� ����(mh_socket)�� �ڵ��� wParam�� ����
//Ŭ���̾�Ʈ�� ������ �ϳ��̱� ������ wParam�� ���ʿ� ����.
//lParam�� ��̺�Ʈ������ FD_CONNECT�� ������ �߻��ߴ��� ����.
//�̺�Ʈ�� �ϳ� ���̶� �����߻��ߴ����� Ȯ���Ѵ�.
{
	if (WSAGETSELECTERROR(lParam)) {
		AddEvent("�������ῡ �����Ͽ����ϴ�.");
		m_connected_flag = 0;
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
	}

	else {
		AddEvent("�������ῡ �����Ͽ����ϴ�.");
	//�� ������ ������ ������ ���������̹Ƿ� �̶����� ���Ͽ� �����͸� �� �� �ִ�.

		m_connected_flag = 2;
		//������ �����Ͽ� ������ ����� ������ ����
		//flag���� 2���� Ȯ���ϰ� �����͸� �����ϵ��� �� �� �̴�.

		WSAAsyncSelect(mh_socket, m_hWnd, 28002, FD_READ | FD_CLOSE);
		//������ �ϳ��� wParam == mh_socket�̹Ƿ� wParam�� ���ʿ䰡 ����.
	}
	return 0;
}

void CMyClientDlg::AddEvent(CString a_string)
//�Ѿ�� ���ڿ��� ����Ʈ �ڽ��� �߰��� �Լ�
//����ڰ� ���ڿ��� ����� �Լ�
{
	while (m_event_list.GetCount() > 1000) {
		m_event_list.DeleteString(0);
	}//ȣ���� ������ 1000�� �̻��־ �Լ��� ����ǵ��� �Ѵ�.
	 //if���̸� ��������

	int index = m_event_list.InsertString(-1, a_string);
	m_event_list.SetCurSel(index);
}

void CMyClientDlg::SendFrameData(SOCKET ah_socket,unsigned char a_msg_id, const char *ap_data, unsigned short int a_data_size)
//� ��������,� �޼�������, ������͸�, �󸶸�ŭ �������� �Ű������� ���޵ȴ�.
{
	int send_data_size = a_data_size + 4;
	char *p_send_data = new char[send_data_size];//��������� ������ ũ�⸦ ����
	
	*p_send_data = 27; //��ȿ�� �޼������� Ȯ�ν�Ŵ //ù��° ���
	*(p_send_data + 1) = a_msg_id; // �ι�° 1����Ʈ�� msg_id
	*(unsigned short int*)(p_send_data + 2) = a_data_size; // �������� ������ 2����Ʈ�� ĳ�����Ѵ�.
	memcpy(p_send_data + 4, ap_data, a_data_size);
	//ap_data�� a_data_size�� ũ�⸸ŭ �����ؼ� p_send_data[4]�� �����ϰڴ�.
	send(mh_socket, p_send_data, send_data_size, 0);


	delete[] p_send_data; 
}

void CMyClientDlg::OnBnClickedSendBtn()
//������ ���� �Լ�
{
	if (m_connected_flag == 2) {
		CString text, str;
		GetDlgItemText(IDC_CHAT_EDIT, text);
		//����Ʈ�ڽ����� ������ �����´�
		str = client_name;
		str += " : ";
		str += text;

		SendFrameData(mh_socket, 1, str, str.GetLength()+1);
		//mh_sockey���� �ż������̵� 1�� ������ str�� ������ �������̴�, str�� ������� str.GetLength�ε�
		//�齽������ �� ����� �����Ƿ� 1�� �����ش�
		SetDlgItemText(IDC_CHAT_EDIT, ""); //����ġ�� ���� �����͸� ������ ��ĭ���� �ʱ�ȭ�Ѵ�.

	}//������ �����͸� ���ش�
}


afx_msg LRESULT CMyClientDlg::On28002(WPARAM wParam, LPARAM lParam)
//READ �޼����� ó���� �Լ� RECV
//�����͸� ������ ����
{
	if (WSAGETSELECTEVENT(lParam) == FD_READ) {
		WSAAsyncSelect(mh_socket, m_hWnd, 28002, FD_CLOSE);
		
		unsigned char check, msg_id;
		unsigned short int body_size;

		recv(mh_socket, (char*)&check, 1, 0); 
		//ù��° ����Ʈ�� 27�ǰ��� ������ Ȯ���Ѵ�.
		CString str;

		if (check == 27) {
			recv(mh_socket, (char*)&msg_id, 1, 0);
			recv(mh_socket, (char*)&body_size, 2, 0);
			if (body_size > 0) {
				char *p_body_data = new char[body_size];
				recv(mh_socket, p_body_data, body_size, 0);

				if (msg_id == 0) {
					//client ����
					client_name = "Client";
					client_name += p_body_data;
					m_step_count = _ttoi(p_body_data);
				}

				else if (msg_id == 1) {
					//ä�� �����͸� recv
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
					printf("��������\n");
				}

				delete[] p_body_data;
			}// �����͸� �޴� �ڵ�

			WSAAsyncSelect(mh_socket, m_hWnd, 28002, FD_READ | FD_CLOSE);
		}
	}
	else
	{
		AddEvent("���������� ���������ϴ�.");
		closesocket(mh_socket);
		mh_socket = INVALID_SOCKET;
		m_connected_flag = 0;
	}
	return 0;
}


void CMyClientDlg::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
    //	CDialogEx::OnOK(); //ok��ư�� Ŭ���ϸ� ����ȴٴ� �ǹ̷� ������ ������� �ʴ´�.
}


void CMyClientDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CClientDC dc(this);
	if (m_step_count % 2 == 0) {
		CString str;

		int x = point.x / LINE_INTERVAL;
		int y = point.y / LINE_INTERVAL;

		if (point.x % LINE_INTERVAL > LINE_INTERVAL / 2) x++;
		if (point.y % LINE_INTERVAL > LINE_INTERVAL / 2) y++;

		int pos = 100 * y + x;

		if (x > 0 && y > 0 && x <= MAX_X_COUNT &&y <= MAX_Y_COUNT) {
			if (m_dol_pos[y - 1][x - 1] == 0) {  //�ش� ��ǥ�� ���� ���ٸ� if���� �����Ѵ�.

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
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	CDialogEx::OnCancel();
}
