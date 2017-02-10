
// MyClientDlg.h : 헤더 파일
//

#pragma once
#include "afxwin.h"

#define MAX_X_COUNT 11
#define MAX_Y_COUNT 11  //줄기준으로 11줄이다 사각형은 100개 그린다.

#define LINE_INTERVAL 40 


// CMyClientDlg 대화 상자
class CMyClientDlg : public CDialogEx
{
private:
	int m_step_count;	//돌 놓는 순서

	CListBox m_event_list;
	CString client_name;
	SOCKET mh_socket;
	CBrush m_white_brush, m_black_brush;

	char m_connected_flag;
	char m_dol_pos[MAX_Y_COUNT][MAX_X_COUNT]; //o : 돌없음, 1 : 검은색, 2 : 흰색
											  //= {{0, }, };  2차원 배열의 생략형

public:
	CMyClientDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

	inline void AddEvent(CString a_string); //inline은 매크로함수로 번역시켜준다.
	void SendFrameData(SOCKET ah_socket, unsigned char a_msg_id, const char *ap_data, unsigned short int a_data_size);

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYCLIENT_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	//각각 28001, 28002 메세지가 발생했을 때 실행되는 함수
	afx_msg LRESULT On28001(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT On28002(WPARAM wParam, LPARAM lParam);

public:
	afx_msg void OnBnClickedSendBtn();
	afx_msg void OnBnClickedOk();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedCancel();
};
