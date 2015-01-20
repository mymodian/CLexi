
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

#include "LxSrcFontFactory.h"

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
	ON_MESSAGE(LEXI_INIT, &CCLexiView::OnLexiInit)
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
	bInitialized = FALSE;
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

void CCLexiView::create_caret(int height, int width)
{
	CreateSolidCaret(width, height);
}
void CCLexiView::hide_caret()
{
	HideCaret();
}
void CCLexiView::show_caret(int x, int y)
{
	POINT point;
	point.x = x;
	point.y = y;
	SetCaretPos(point);
	ShowCaret();
}

void CCLexiView::OnDraw(CDC* pDC)
{
	CCLexiDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	/*CreateSolidCaret(2, 16);
	SetCaretPos(CPoint(100, 100));
	ShowCaret();*/

	//CRect rect;
	//GetClientRect(&rect);
	//CDC dcMem;
	//CBitmap bmp;

	//dcMem.CreateCompatibleDC(pDC);
	//bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	//dcMem.SelectObject(&bmp);
	//dcMem.FillSolidRect(rect, pDC->GetBkColor());
	//dcMem.TextOut(100, 100, "hello world!!!");

	//CString screen_size;
	//int cx = GetSystemMetrics(SM_CXSCREEN);
	//int cy = GetSystemMetrics(SM_CYSCREEN);
	//
	//int nScreenWidth, nScreenHeight;
	//HDC hdcScreen = ::GetDC(NULL);			//获取屏幕的HDC
	//nScreenWidth = GetDeviceCaps(hdcScreen, HORZSIZE);
	//nScreenHeight = GetDeviceCaps(hdcScreen, VERTSIZE);
	//screen_size.Format("分辨率:%d,%d   尺寸:%d,%d", cx, cy, nScreenWidth, nScreenHeight);
	//dcMem.TextOutA(100, 200, screen_size);

	//pDC->BitBlt(0, 0, rect.Width(), rect.Height(),
	//	&dcMem, 0, 0, SRCCOPY);

	//dcMem.DeleteDC();
	//bmp.DeleteObject();
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

LRESULT CCLexiView::OnLexiInit(WPARAM wParam, LPARAM lParam)
{
	CDC* pDC = GetDC();
	doc_view_controler.init(pDC);
	doc_view_controler.draw_complete(pDC);
	ReleaseDC(pDC);
	bInitialized = TRUE;
	return 0;
}

BOOL CCLexiView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CView::OnEraseBkgnd(pDC);
}

void CCLexiView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	static TCHAR character = 0;
	switch (nChar)
	{
	case '\t':
		//table 转为多个空格处理
	{
		TCHAR cs[4] = { ' ', ' ', ' ', ' ' };
		this->insert(cs, 4);
	}
		break;
	case '\r':
		//按下回车，创建新物理段
		break;
	case '\n':
		break;
	case 8:
		//删除
		backspace();
		break;
	default:
		//字符输入
	{
		if (nChar & 0x80 != 1)
		{
			character = nChar;
			this->insert(&character, 1);
			character = 0;
		}
		else
		{
			character <<= 8;
			character |= nChar;
			if (character & 0xff00)
			{
				this->insert(&character, 1);
				character = 0;
			}
		}
	}
		break;
	}
	CView::OnChar(nChar, nRepCnt, nFlags);
}


void CCLexiView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	LxCommand* locate_cmd = new LxCommand();
	locate_cmd->add_child_cmd(new LxLocateCmd(point.x, point.y));
	locate_cmd->set_dvctl(&doc_view_controler);
	//ExecuteNormalCmd(locate_cmd);
	CDC* pDC = GetDC();
	locate_cmd->Excute(pDC);
	ReleaseDC(pDC);
	lx_command_mgr.insert_cmd(locate_cmd);

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
	//size改变后，文档的offset_y保持不变，但当客户区域的高度大于等于文档总高度时，将offset_y设为0
	//同样offset_x也保持不变，当客户区域的宽度大于等于文档页面宽度时，将offset_x设为0
	if (!bInitialized)
		return;
	CDC* pDC = GetDC();
	doc_view_controler.draw_complete(pDC);
	ReleaseDC(pDC);
}

// CCLexiView mydoc命令处理程序

void CCLexiView::ExecuteNormalCmd(LxCommand* cmd)
{
	CRect rect;
	GetClientRect(&rect);
	CDC dcMem;
	CBitmap bmp;
	CDC* pDC = GetDC();
	dcMem.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	dcMem.SelectObject(&bmp);
	dcMem.FillSolidRect(rect, pDC->GetBkColor());

	cmd->Excute(&dcMem);

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(),
		&dcMem, 0, 0, SRCCOPY);

	dcMem.DeleteDC();
	bmp.DeleteObject();
	ReleaseDC(pDC);
}

void CCLexiView::insert(TCHAR* cs, int len)
{
	LxCommand* insert_cmd = new LxCommand();
	insert_cmd->add_child_cmd(new LxInsertCmd(cs, len));
	insert_cmd->set_dvctl(&doc_view_controler);
	/*CDC* pDC = GetDC();
	insert_cmd->Excute(pDC);
	ReleaseDC(pDC);*/
	ExecuteNormalCmd(insert_cmd);
	lx_command_mgr.insert_cmd(insert_cmd);
}

void CCLexiView::backspace()
{
	LxCommand* backspace_cmd = new LxCommand();
	backspace_cmd->add_child_cmd(new LxSingleRemoveCmd());
	backspace_cmd->set_dvctl(&doc_view_controler);
	/*CDC* pDC = GetDC();
	insert_cmd->Excute(pDC);
	ReleaseDC(pDC);*/
	ExecuteNormalCmd(backspace_cmd);
	lx_command_mgr.insert_cmd(backspace_cmd);
}