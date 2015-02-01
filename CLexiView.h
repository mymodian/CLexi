
// CLexiView.h : CCLexiView 类的接口
//

#pragma once

#define LEXI_INIT WM_USER + 0x01

#include "LxDcViCtl.h"
#include "ChromiumTask.h"

class CCLexiView : public CView
{
private:
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
	int GetTextTrueWidth(TCHAR* text, size_t src_font);
	int GetTextTrueHeight(TCHAR text, size_t src_font);

public:
	void create_caret(int height, int width);
	void hide_caret();
	void show_caret(int x, int y);
	void move_caret(unsigned int direction);
public:
	void ExecuteNormalTask(Task<CDC>* task);
public:
	void insert(TCHAR* cs, int len);
	void wrap();
	void backspace();
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
	long IMECharLeft;
	long IMECharSize;
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
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};

#ifndef _DEBUG  // CLexiView.cpp 中的调试版本
inline CCLexiDoc* CCLexiView::GetDocument() const
   { return reinterpret_cast<CCLexiDoc*>(m_pDocument); }
#endif

