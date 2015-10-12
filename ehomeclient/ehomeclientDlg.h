// ehomeclientDlg.h : 头文件
//

#include "nbEhomeClientNew.h"
#include "ehomeUdpListener.h"

#pragma once

//define lamp id

#define OFFICE_LAMP1  1
#define OFFICE_LAMP2  2
#define OFFICE_LAMP3  3
#define OFFICE_LAMP4  4
#define OFFICE_LAMP5  5
#define OFFICE_LAMP6  6

#define EXAMPLE_LAMP1     7
#define EXAMPLE_LAMP2     8
#define EXAMPLE_LAMP3     9
#define EXAMPLE_LAMP4     10
#define EXAMPLE_LAMP5_1   11

#define EXAMPLE_LAMP5_2   12  //调光

#define EXAMPLE_LAMP6     13
#define EXAMPLE_LAMP7     14
#define EXAMPLE_LAMP8     15
#define EXAMPLE_LAMP9     16

//窗帘
#define EXAMPLE_CURETAIN1_1     17
#define EXAMPLE_CURETAIN1_2     18
#define EXAMPLE_CURETAIN2_1     19
#define EXAMPLE_CURETAIN2_2     20

//定时器
#define TIMER_READ_STATE  1

//界面id对应信息
typedef struct {
    int iDevId;
    int iIdcStateId;
} UIResInfo, *PUIResInfo;


// CehomeclientDlg 对话框
class CehomeclientDlg : public CDialog
{
private:
    CNbEhomeTcpClientNew m_ehclt;
    CEhomeUdpListener m_udpListener;

    int m_arrChecks[20];

private:
    void WriteDev(int iDevNo, int iVal);
    void GetChecks(void);

    void SendReadAllDevCmd(void);

    PUIResInfo GetResInfoFromID(int iID);

// 构造
public:
	CehomeclientDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_EHOMECLIENT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedBtnConnect();
    afx_msg void OnBnClickedBtnOpen1();
    afx_msg void OnBnClickedBtnClose1();
    afx_msg void OnBnClickedBtnOpen2();
    afx_msg void OnBnClickedBtnClose2();
    afx_msg void OnBnClickedBtnOpen3();
    afx_msg void OnBnClickedBtnClose3();
    afx_msg void OnBnClickedBtnOpen4();
    afx_msg void OnBnClickedBtnClose4();
    afx_msg void OnBnClickedBtnOpen5();
    afx_msg void OnBnClickedBtnClose5();
    afx_msg void OnBnClickedBtnOpen6();
    afx_msg void OnBnClickedBtnClose6();
    afx_msg void OnBnClickedBtnOpen7();
    afx_msg void OnBnClickedBtnClose7();
    afx_msg void OnBnClickedBtnOpen8();
    afx_msg void OnBnClickedBtnClose8();
    afx_msg void OnBnClickedBtnOpen9();
    afx_msg void OnBnClickedBtnClose9();
    afx_msg void OnBnClickedBtnOpen10();
    afx_msg void OnBnClickedBtnClose10();
    afx_msg void OnBnClickedBtnOpen11();
    afx_msg void OnBnClickedBtnClose11();
    afx_msg void OnBnClickedBtnOpen12();
    afx_msg void OnBnClickedBtnOpen13();
    afx_msg void OnBnClickedBtnClose13();
    afx_msg void OnBnClickedBtnOpen14();
    afx_msg void OnBnClickedBtnClose14();
    afx_msg void OnBnClickedBtnOpen15();
    afx_msg void OnBnClickedBtnClose15();
    afx_msg void OnBnClickedBtnOpen16();
    afx_msg void OnBnClickedBtnClose16();
    afx_msg void OnBnClickedBtnOpen17();
    afx_msg void OnBnClickedBtnClose17();
    afx_msg void OnBnClickedBtnOpen18();
    afx_msg void OnBnClickedBtnClose18();
    afx_msg void OnBnClickedBtnOpen19();
    afx_msg void OnBnClickedBtnClose19();
    afx_msg void OnBnClickedBtnOpen20();
    afx_msg void OnBnClickedBtnClose20();
    afx_msg void OnBnClickedBtnBatopen();
    afx_msg void OnBnClickedBtnBatclose();
    afx_msg void OnTimer(UINT_PTR nIDEvent);

    afx_msg LRESULT OnReadMessage(WPARAM wParam, LPARAM lParam);  
    afx_msg void OnBnClickedBtnGetjson();
};
