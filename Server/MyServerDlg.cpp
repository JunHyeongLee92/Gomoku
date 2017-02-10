
// MyServerDlg.cpp : ���� ����
//

#include "stdafx.h"
#include "MyServer.h"
#include "MyServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CCriticalSection g_cs;

// CMyServerDlg ��ȭ ����


CMyServerDlg::CMyServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_MYSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	for (int i = 0; i < MAX_COUNT; i++) {
		m_client_list[i].h_socket = INVALID_SOCKET; 
		//� ���� ��� ������ invalid_socket�� ����ִ� ���� ����̰� �ٸ����� �ִ¹��� �ǳʶٵ��� �Ѵ�.
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


// CMyServerDlg �޽��� ó����

BOOL CMyServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// �� ��ȭ ������ �������� �����մϴ�.  ���� ���α׷��� �� â�� ��ȭ ���ڰ� �ƴ� ��쿡��
	//  �����ӿ�ũ�� �� �۾��� �ڵ����� �����մϴ�.
	SetIcon(m_hIcon, TRUE);			// ū �������� �����մϴ�.
	SetIcon(m_hIcon, FALSE);		// ���� �������� �����մϴ�.

	mh_socket = socket(AF_INET, SOCK_STREAM, 0); //AF_INET = INET������� ����� �ϰڴ�.
		//0���� �س����� ���Ÿ�Կ� �´� ���������� �˾Ƽ� ã���ش�
		//��ȣ���� 32��Ʈ unsigned int�� �̸��� �ٲ۰� = socket
	sockaddr_in srv_addr; //srv_addr�� �����ϱ����� ������ ������� �����ϸ�ȴ�.
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_addr.s_addr = INADDR_ANY; //������ address
	srv_addr.sin_port = htons(15003); //�����α׷��ȿ��� � ��ǻ�Ϳ� ��������� ���Ѵ�
	//port�� �ߺ��Ǽ� �ȵȴ�. �����������ڰ� ����
	//��Ʈ��ȣ�� 15000���̻� ����

	bind(mh_socket, (LPSOCKADDR)&srv_addr, sizeof(srv_addr));
	listen(mh_socket, 1); 
	//1�� ���ÿ� call���� �� �ִ� ����, �� �ѹ��� �Ѹ��Ը� ���� ���� �� �ִ�.

	AfxBeginThread(AcceptFunc, this);

	AddEvent("���� ���񽺸� �����մϴ�.");

	return TRUE;  // ��Ŀ���� ��Ʈ�ѿ� �������� ������ TRUE�� ��ȯ�մϴ�.
}

// ��ȭ ���ڿ� �ּ�ȭ ���߸� �߰��� ��� �������� �׸�����
//  �Ʒ� �ڵ尡 �ʿ��մϴ�.  ����/�� ���� ����ϴ� MFC ���� ���α׷��� ��쿡��
//  �����ӿ�ũ���� �� �۾��� �ڵ����� �����մϴ�.

void CMyServerDlg::OnPaint()
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
		CDialogEx::OnPaint();
	}
}

// ����ڰ� �ּ�ȭ�� â�� ���� ���ȿ� Ŀ���� ǥ�õǵ��� �ý��ۿ���
//  �� �Լ��� ȣ���մϴ�.
HCURSOR CMyServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

//Ŭ���̾�Ʈ�� ����ؼ� �޾��ִ� �����忡 ����� �Լ�
UINT CMyServerDlg::AcceptFunc(LPVOID pParam)
{
	CMyServerDlg *pDlg = (CMyServerDlg*)pParam;


	while (true)
	{
		//�̰��� ������ �ݺ��ϸ� ������ ������ �ֽ��ϴ�.
		SOCKADDR_IN addr;
		memset(&addr, 0, sizeof(addr));
		int iAddrLen = sizeof(addr), i;

		SOCKET ClientSocket = accept(pDlg->mh_socket, (sockaddr*)&addr, &iAddrLen);
		if (ClientSocket == INVALID_SOCKET)
			break;

		//������ ����ȭ �Ӱ迵��
	
		g_cs.Lock();
		for (i = 0; i < MAX_COUNT; i++) {
			if (pDlg->m_client_list[i].h_socket == INVALID_SOCKET) break;
		}
		if (i < MAX_COUNT) { //����� �������� accept�� �Ѵ�.
			pDlg->m_client_list[i].h_socket = ClientSocket;
			//sockaddr_in_size�� �������� �α׸� �����ش�		
			strcpy(pDlg->m_client_list[i].ip_address, inet_ntoa(addr.sin_addr));
			//ip�ּ� ����

			CString str = "���ο� ����ڰ� �����߽��ϴ� : ";
			str += pDlg->m_client_list[i].ip_address; //���ο� ����ڰ� �����ϸ� ip�� ����ش�.

			pDlg->AddEvent(str);

			pDlg->SendFrameData(pDlg->m_client_list[i].h_socket, 1, str, str.GetLength() + 1);

			str.Format(_T("%d"), i);	//client ��ȣ ����
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

//Ŭ���̾�Ʈ�� ���� �޼����� �޾��ִ� �����忡 ����ϴ� �Լ�
UINT CMyServerDlg::RecvFunc(LPVOID pParam)
{
	CRecvParam *p = (CRecvParam*)pParam;
	CMyServerDlg *pDlg = p->m_pDlg;
	SOCKET ClientSocket = p->m_ClientSocket;;

	while (true) {

		unsigned char check, msg_id;//message_id
		unsigned short int body_size;

		recv(ClientSocket, (char*)&check, 1, 0); //��� 1����Ʈ�� ���� 
													 //��� ��ȿ�� üũ�ϴ� �κ�
		if (check == 27) {
			recv(ClientSocket, (char*)&msg_id, 1, 0); //�޼��� ���� �κ��� ����
			recv(ClientSocket, (char*)&body_size, 2, 0); //body�� ���� �� �ִ� ������ �κ��� ����
			if (body_size > 0) {
				char *p_body_data = new char[body_size]; //�ٵ������ �о�ͼ� �����Ҵ��Ѵ�.
				recv(ClientSocket, p_body_data, body_size, 0);
				CString str;
				if (msg_id == 1) {
					str = p_body_data;
					pDlg->AddEvent(str);
					for (int i = 0; i < MAX_COUNT; i++) {
						if (pDlg->m_client_list[i].h_socket != INVALID_SOCKET) {
							pDlg->SendFrameData(pDlg->m_client_list[i].h_socket, 1, str, str.GetLength() + 1);
							//�ڱ����� �پ��ִ� ��� Ŭ���̾�Ʈ���� ���� �����͸� �������ش�.
						}
					}
				}
				else if (msg_id == 2) {
					//�ٵϵ� ��ǥ ������ recv

					str = p_body_data;
					int pos = _ttoi(str);	//CString to integer
					
					if (pDlg->m_step_count % 2)
						pDlg->color = 2;
					else
						pDlg->color = 1;
					pos += pDlg->color * 10000;	//��ǥ���� �ٵϵ������� ����
					
					str.Format(_T("%d"), pos);
					pDlg->m_step_count++;
					for (int i = 0; i < MAX_COUNT; i++) {
						if (pDlg->m_client_list[i].h_socket != INVALID_SOCKET) {
							//�ڱ����� �پ��ִ� ��� Ŭ���̾�Ʈ���� ���� �����͸� �������ش�.
							pDlg->SendFrameData(pDlg->m_client_list[i].h_socket, 2, str, str.GetLength() + 1);
							// �ٵϵ� ��ǥ�� ����
						}
					}
				}
				delete[] p_body_data; //���� �� �����Ҵ� �������ش�.
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
	}//ȣ���� ������ 1000�� �̻��־ �Լ��� ����ǵ��� �Ѵ�.
	 //if���̸� ��������

	int index = m_event_list.InsertString(-1, a_string);
	m_event_list.SetCurSel(index);
}

//������ �����͸� �ְ������ ������ȭ �����ִ� �Լ�
void CMyServerDlg::SendFrameData(SOCKET ah_socket, unsigned char a_msg_id, const char *ap_data, unsigned short int a_data_size)
//� ��������,� �޼�������, ������͸�, �󸶸�ŭ �������� �Ű������� ���޵ȴ�.
{
	int send_data_size = a_data_size + 4;
	char *p_send_data = new char[send_data_size];//��������� ������ ũ�⸦ ����

	*p_send_data = 27; //��ȿ�� �޼������� Ȯ�ν�Ŵ //ù��° ���
	*(p_send_data + 1) = a_msg_id; // �ι�° 1����Ʈ�� msg_id
	*(unsigned short int*)(p_send_data + 2) = a_data_size; // �������� ������ 2����Ʈ�� ĳ�����Ѵ�.
	memcpy(p_send_data + 4, ap_data, a_data_size);
	//ap_data�� a_data_size�� ũ�⸸ŭ �����ؼ� p_send_data[4]�� �����ϰڴ�.
	send(ah_socket, p_send_data, send_data_size, 0);

	delete[] p_send_data;
}

//�񵿱�Ŀ��� ����ϴ� �Լ��� �����忡�� �������ʴ´�
afx_msg LRESULT CMyServerDlg::On25001(WPARAM wParam, LPARAM lParam)
//�̸޼����� �߻��ϰԵ� ������ ������ �ڵ��� wParam�� ���޵ȴ�.
//lParam�� 32��Ʈ�ε� 16��Ʈ�� ��޼������� , ������ 16��Ʈ�� ������ �߻��ߴ��� ����ִ�.
{
	//SOCKET h_socket = (SOCKET)wParam;

	//sockaddr_in client_addr; //ip, ���� �ڵ��� ����ؾ��Ѵ�.
	//int sockaddr_in_size = sizeof(sockaddr_in), i;
	//
	//for (i = 0; i < MAX_COUNT; i++) {
	//	if (m_client_list[i].h_socket == INVALID_SOCKET) break;
	//}
	//if (i < MAX_COUNT) { //����� �������� accept�� �Ѵ�.
	//	m_client_list[i].h_socket = accept(h_socket, (LPSOCKADDR)&client_addr, &sockaddr_in_size);
	//	//sockaddr_in_size�� �������� �α׸� �����ش�		
	//	strcpy(m_client_list[i].ip_address, inet_ntoa(client_addr.sin_addr)); 
	//	//ip�ּ� ����

	//	CString str = "���ο� ����ڰ� �����߽��ϴ� : ";
	//	str += m_client_list[i].ip_address; //���ο� ����ڰ� �����ϸ� ip�� ����ش�.
	//	
	//	AddEvent(str);

	//	SendFrameData(m_client_list[i].h_socket, 1, str, str.GetLength() + 1);

	//	str.Format(_T("%d"), i);	//client ��ȣ ����
	//	AddEvent(str);
	//	SendFrameData(m_client_list[i].h_socket, 0, str, str.GetLength() + 1);

	//	WSAAsyncSelect(m_client_list[i].h_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
	//	//�񵿱�
	//	//���������쿡�� ���� accpet�� ���Ͽ�
	//	//������� �����͸� �ְų�(fd_read) ���� ������(fd_close) �޼��� 25002�� ���
	//}
	//else {
	//	closesocket(h_socket);
	//}

	return 0;
}

afx_msg LRESULT CMyServerDlg::On25002(WPARAM wParam, LPARAM lParam)
//lParam : � ���������� �� �޼����� �߻��ߴ°�
//�����͸� READ�Ҷ� ����Ǵ� �Լ�
{
	//ClientSocket *p_user;

	//for (int i = 0; i < MAX_COUNT; i++) {
	//	if (m_client_list[i].h_socket == wParam) {
	//		/*if(m_client_list[i].h_socket != INVALID_SOCKET &&
	//		m_client_list[i].h_socket == wParam) */
	//		p_user = &m_client_list[i];
	//		/*int A[3]�� ������ ���� �Ź�  *(A+3)���� ����ż� ���������� ���Ƿ�
	//		�����͸� �ϳ� ����� �ּҸ� �޾� ���°� �̵��̴�.
	//		int *p;   p = &A[3];  */
	//		break;
	//	}
	//}
	//if (WSAGETSELECTEVENT(lParam) == FD_READ)//������� ������ �����͸� ������ 25002�޼��� �߻�
	//{
	//	WSAAsyncSelect(p_user->h_socket, m_hWnd, 25002, FD_CLOSE);
	//	//���� �б⸦ �Ұ� �̱� ������ 25002�� ���ѹ߻��ϴ� ����
	//	//���� ���� �дµ��� ������ ���� �񵿱⸦ �����ش�.

	//	unsigned char check, msg_id;//message_id
	//	unsigned short int body_size;

	//	recv(p_user->h_socket, (char*)&check, 1, 0); //��� 1����Ʈ�� ���� 
	//												 //��� ��ȿ�� üũ�ϴ� �κ�
	//	if (check == 27) {
	//		recv(p_user->h_socket, (char*)&msg_id, 1, 0); //�޼��� ���� �κ��� ����
	//		recv(p_user->h_socket, (char*)&body_size, 2, 0); //body�� ���� �� �ִ� ������ �κ��� ����

	//		if (body_size > 0) {
	//			char *p_body_data = new char[body_size]; //�ٵ������ �о�ͼ� �����Ҵ��Ѵ�.

	//			recv(p_user->h_socket, p_body_data, body_size, 0);

	//			CString str;

	//			if (msg_id == 1) {

	//				str = p_body_data;
	//				AddEvent(str);

	//				for (int i = 0; i < MAX_COUNT; i++) {
	//					if (m_client_list[i].h_socket != INVALID_SOCKET) {
	//						SendFrameData(m_client_list[i].h_socket, 1, str, str.GetLength() + 1);
	//						//�ڱ����� �پ��ִ� ��� Ŭ���̾�Ʈ���� ���� �����͸� �������ش�.
	//					}
	//				}

	//			}
	//			else if (msg_id == 2) {
	//				//�ٵϵ� ��ǥ ������ recv

	//				str = p_body_data;
	//				int pos = _ttoi(str);	//CString to integer

	//				int y = pos / 100;
	//				int x = pos % 100;

	//				if (m_step_count % 2)
	//					color = 2;
	//				else
	//					color = 1;

	//				pos += color * 10000;	//��ǥ���� �ٵϵ������� ����
	//				str.Format(_T("%d"), pos);
	//				m_step_count++;

	//				for (int i = 0; i < MAX_COUNT; i++) {
	//					if (m_client_list[i].h_socket != INVALID_SOCKET) {
	//						//�ڱ����� �پ��ִ� ��� Ŭ���̾�Ʈ���� ���� �����͸� �������ش�.
	//						SendFrameData(m_client_list[i].h_socket, 2, str, str.GetLength() + 1); // �ٵϵ� ��ǥ�� ����
	//					}
	//				}
	//			}
	//			delete[] p_body_data; //���� �� �����Ҵ� �������ش�.
	//		}

	//		WSAAsyncSelect(p_user->h_socket, m_hWnd, 25002, FD_READ | FD_CLOSE);
	//		//�۾��� ���ϰ� �ٽ� FD_READ�� ���� �񵿱⸦ �ɾ��ش�.
	//	}
	//}

	//else { //������� ���� ������ ��

	//	   //closesocket((SOCKET)wParam); //������ ���� ������ �ּҸ� �ҷ��;��Ѵ�
	//	closesocket(p_user->h_socket); //m_client_list[i].h_socket		
	//	p_user->h_socket = INVALID_SOCKET; //������ ���� �濡 INVALID_SOCKET���� �־��ش�.
	//}
	return 0;
}