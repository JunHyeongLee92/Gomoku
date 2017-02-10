
// MyClientDlg.h : ��� ����
//

#pragma once
#include "afxwin.h"

#define MAX_X_COUNT 11
#define MAX_Y_COUNT 11  //�ٱ������� 11���̴� �簢���� 100�� �׸���.

#define LINE_INTERVAL 40 


// CMyClientDlg ��ȭ ����
class CMyClientDlg : public CDialogEx
{
private:
	int m_step_count;	//�� ���� ����

	CListBox m_event_list;
	CString client_name;
	SOCKET mh_socket;
	CBrush m_white_brush, m_black_brush;

	char m_connected_flag;
	char m_dol_pos[MAX_Y_COUNT][MAX_X_COUNT]; //o : ������, 1 : ������, 2 : ���
											  //= {{0, }, };  2���� �迭�� ������

public:
	CMyClientDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.

	inline void AddEvent(CString a_string); //inline�� ��ũ���Լ��� ���������ش�.
	void SendFrameData(SOCKET ah_socket, unsigned char a_msg_id, const char *ap_data, unsigned short int a_data_size);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYCLIENT_DIALOG };
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
	//���� 28001, 28002 �޼����� �߻����� �� ����Ǵ� �Լ�
	afx_msg LRESULT On28001(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On28002(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnBnClickedSendBtn();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCancel();
};
