
// CLexiView.cpp : CCLexiView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "CLexi.h"
#endif

#include "CLexiDoc.h"
#include "CLexiView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCLexiView

IMPLEMENT_DYNCREATE(CCLexiView, CView)

BEGIN_MESSAGE_MAP(CCLexiView, CView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

// CCLexiView 构造/析构

CCLexiView::CCLexiView()
{
	// TODO: 在此处添加构造代码

}

CCLexiView::~CCLexiView()
{
}

BOOL CCLexiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CCLexiView 绘制

void CCLexiView::OnDraw(CDC* pDC)
{
	CCLexiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	CreateSolidCaret(2, 16);
	SetCaretPos(CPoint(100, 100));
	ShowCaret();

	CRect rect;
	GetClientRect(&rect);
	CDC dcMem;
	CBitmap bmp;

	dcMem.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	dcMem.SelectObject(&bmp);
	dcMem.FillSolidRect(rect, pDC->GetBkColor());
	dcMem.TextOut(100, 100, "hello world!!!");
	pDC->BitBlt(0, 0, rect.Width(), rect.Height(),
		&dcMem, 0, 0, SRCCOPY);

	dcMem.DeleteDC();
	bmp.DeleteObject();
}


// CCLexiView 打印

BOOL CCLexiView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CCLexiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CCLexiView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
}


// CCLexiView 诊断

#ifdef _DEBUG
void CCLexiView::AssertValid() const
{
	CView::AssertValid();
}

void CCLexiView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CCLexiDoc* CCLexiView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCLexiDoc)));
	return (CCLexiDoc*)m_pDocument;
}
#endif //_DEBUG


// CCLexiView 消息处理程序

BOOL CCLexiView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}

void CCLexiView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CView::OnChar(nChar, nRepCnt, nFlags);
}


void CCLexiView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CView::OnLButtonDown(nFlags, point);
}


void CCLexiView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CView::OnLButtonUp(nFlags, point);
}


void CCLexiView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CView::OnMouseMove(nFlags, point);
}


void CCLexiView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);
	CRect rect;
	GetClientRect(&rect);
	AdjustViewWindow(rect.Width(), rect.Height());
	// TODO:  在此处添加消息处理程序代码
}