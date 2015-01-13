
// CLexiView.h : CCLexiView 类的接口
//

#pragma once

#define LEXI_INIT WM_USER + 0x01

#include "LxCommand.h"

class CCLexiView : public CView
{
private:
	LxCommandMgr lx_command_mgr;		//命令管理
	LxDcViCtl doc_view_controler;				//文档和排版控制

protected: // 仅从序列化创建
	CCLexiView();
	DECLARE_DYNCREATE(CCLexiView)

// 特性
public:
	CCLexiDoc* GetDocument() const;

// 操作
public:
//获取信息
public:
	int GetTextTrueWidth(char* text, size_t src_font);
	int GetTextTrueHeight(char text, size_t src_font);

public:
	void create_caret(int height, int width);
	void hide_caret();
	void show_caret(int x, int y);
public:
	void insert(char c);

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 实现
private:

//状态
private:
	BOOL bInitialized;
public:
	virtual ~CCLexiView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg LRESULT OnLexiInit(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // CLexiView.cpp 中的调试版本
inline CCLexiDoc* CCLexiView::GetDocument() const
   { return reinterpret_cast<CCLexiDoc*>(m_pDocument); }
#endif

