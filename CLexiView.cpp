
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

#include <imm.h>
#pragma comment(lib,"imm32.lib")

#include "LxSrcFontFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCLexiView

IMPLEMENT_DYNCREATE(CCLexiView, CScrollView)

BEGIN_MESSAGE_MAP(CCLexiView, CScrollView)
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CScrollView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CScrollView::OnFilePrintPreview)
	ON_MESSAGE(LEXI_INIT, &CCLexiView::OnLexiInit)
	ON_WM_CHAR()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_SETCURSOR()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(ID_SET_FONT, &CCLexiView::OnSetFont)
	ON_COMMAND(ID_SET_COLOR, &CCLexiView::OnSetColor)
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_FILE_SAVE_AS, &CCLexiView::OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE, &CCLexiView::OnFileSave)
	ON_COMMAND(ID_FILE_OPEN, &CCLexiView::OnFileOpen)
	ON_MESSAGE(WM_HOTKEY, &CCLexiView::OnHotKey)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CCLexiView 构造/析构

CCLexiView::CCLexiView()
{
	// TODO: 在此处添加构造代码
	bInitialized = FALSE;
	IMECharLeft = 0;
	IMECharSize = 0;
	width_total_ = LxPaper::pixel_width;
	height_total_ = LxPaper::pixel_height + 2 * ViewWindow::GetViewWindowInstance()->border_height;
	font_index = 0;
}

CCLexiView::~CCLexiView()
{
}

BOOL CCLexiView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CScrollView::PreCreateWindow(cs);
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
void CCLexiView::move_caret(unsigned int direction)
{
	CDC* pDC = GetDC();
	doc_view_controler.usr_move_cursor(pDC, direction);
	ReleaseDC(pDC);
}

void CCLexiView::set_scroll_size_total(int width_total, int height_total)
{
	SetScrollSizes(MM_TEXT, CSize(width_total, height_total));
	width_total_ = width_total;
	height_total_ = height_total;
}
void CCLexiView::set_scroll_width_total(int width_total)
{
	SetScrollSizes(MM_TEXT, CSize(width_total, height_total_));
	width_total_ = width_total;
}
void CCLexiView::set_scroll_height_total(int height_total)
{
	SetScrollSizes(MM_TEXT, CSize(width_total_, height_total));
	height_total_ = height_total;
}
void CCLexiView::set_scroll_pos(int hscroll_pos, int vscroll_pos)
{
	POINT point;
	point.x = hscroll_pos;
	point.y = vscroll_pos;
	ScrollToPosition(point);
}
void CCLexiView::set_hscroll_pos(int hscroll_pos)
{
	POINT point;
	point.x = hscroll_pos;
	point.y = GetScrollPosition().y;
	ScrollToPosition(point);
}
void CCLexiView::set_vscroll_pos(int vscroll_pos)
{
	POINT point;
	point.y = vscroll_pos;
	point.x = GetScrollPosition().x;
	ScrollToPosition(point);
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
	//dcMem.TextOut(100, 200, screen_size);

	//pDC->BitBlt(0, 0, rect.Width(), rect.Height(),
	//	&dcMem, 0, 0, SRCCOPY);

	//dcMem.DeleteDC();
	//bmp.DeleteObject();
}

void CCLexiView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	// TODO:  在此添加专用代码和/或调用基类
	set_scroll_size_total(LxPaper::pixel_width, LxPaper::pixel_height + 2 * ViewWindow::GetViewWindowInstance()->border_height);
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
	CScrollView::AssertValid();
}

void CCLexiView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CCLexiDoc* CCLexiView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCLexiDoc)));
	return (CCLexiDoc*)m_pDocument;
}
#endif //_DEBUG


// CCLexiView 消息处理程序


BOOL CCLexiView::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类
	if (pMsg->message == WM_IME_ENDCOMPOSITION)
	{
		HIMC himc = ImmGetContext(this->m_hWnd);
		IMECharSize = IMECharLeft = ImmGetCompositionString(himc, GCS_RESULTSTR, 0, 0);
		ImmReleaseContext(this->m_hWnd, himc);
	}
	return CScrollView::PreTranslateMessage(pMsg);
}

BOOL CCLexiView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	return CScrollView::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CCLexiView::OnLexiInit(WPARAM wParam, LPARAM lParam)
{
	CDC* pDC = GetDC();
	doc_view_controler.init(pDC);
	doc_view_controler.draw_complete(pDC);
	ReleaseDC(pDC);
	bInitialized = TRUE;
	//::SetFocus(m_hWnd);
	SetClassLong(GetSafeHwnd(), GCL_HCURSOR, (long)::LoadCursor(NULL, IDC_IBEAM));
	return 0;
}

BOOL CCLexiView::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
	//return CScrollView::OnEraseBkgnd(pDC);
}

void CCLexiView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	switch (nChar)
	{
	case VK_UP:
	case VK_DOWN:
	case VK_LEFT:
	case VK_RIGHT:
		move_caret(nChar);
		break;
	default:
		break;
	}
	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CCLexiView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (IMECharLeft == 0)
	{
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
			wrap();
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
			TCHAR character = nChar;
			this->insert(&character, 1);
		}
			break;
		}
	}
	else
	{
		IMECharLeft -= 2;
		if (IMECharLeft == 0)
		{
			HIMC himc = ImmGetContext(this->m_hWnd);
			TCHAR* result_str = new TCHAR[IMECharSize / 2];
			ImmGetCompositionString(himc, GCS_RESULTSTR, (BYTE*)result_str, IMECharSize);
			ImmReleaseContext(this->m_hWnd, himc);
			this->insert(result_str, IMECharSize / 2);
			delete[] result_str;
		}
	}
	CScrollView::OnChar(nChar, nRepCnt, nFlags);
}


void CCLexiView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDC* pDC = GetDC();
	doc_view_controler.usr_mouse_lbutton_down(pDC, point.x, point.y);
	ReleaseDC(pDC);

	/*CString info;
	info.Format(L"down (%d,%d)\n", point.x, point.y);
	OutputDebugString(info);*/
	//::SetFocus(m_hWnd);
	CScrollView::OnLButtonDown(nFlags, point);
}


void CCLexiView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDC* pDC = GetDC();
	doc_view_controler.usr_mouse_lbutton_up(pDC, point.x, point.y);
	ReleaseDC(pDC);

	/*CString info;
	info.Format(L"up (%d,%d)\n", point.x, point.y);
	OutputDebugString(info);*/
	CScrollView::OnLButtonUp(nFlags, point);
}


void CCLexiView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDC* pDC = GetDC();
	doc_view_controler.usr_mouse_move(pDC, point.x, point.y);
	ReleaseDC(pDC);

	/*CString info;
	info.Format(L"move (%d,%d)\n", point.x, point.y);
	OutputDebugString(info);*/
	CScrollView::OnMouseMove(nFlags, point);
}


void CCLexiView::OnSize(UINT nType, int cx, int cy)
{
	CScrollView::OnSize(nType, cx, cy);
	CRect rect;
	GetClientRect(&rect);
	doc_view_controler.modify_view_size(rect.Width(), rect.Height());

	if (!bInitialized)
		return;
	CDC* pDC = NULL;
	Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::draw_complete);
	ExecuteNormalTask(task, pDC);
	delete task;
}

// CCLexiView mydoc命令处理程序

void CCLexiView::ExecuteNormalTask(Task<CDC>* task, CDC* pdc)
{
	CRect rect;
	GetClientRect(&rect);
	CDC dcMem;
	CBitmap bmp;
	CDC* pDC = pdc ? pdc : GetDC();
	dcMem.CreateCompatibleDC(pDC);
	bmp.CreateCompatibleBitmap(pDC, rect.Width(), rect.Height());
	dcMem.SelectObject(&bmp);
	dcMem.FillSolidRect(rect, pDC->GetBkColor());

	task->run(&dcMem);

	pDC->BitBlt(0, 0, rect.Width(), rect.Height(),
		&dcMem, 0, 0, SRCCOPY);

	dcMem.DeleteDC();
	bmp.DeleteObject();
	if (!pdc)
		ReleaseDC(pDC);
}

void CCLexiView::insert(TCHAR* cs, int len)
{
	size_t src_font = font_index;
	if (font_index == 0)
		src_font = SrcFontFactory::GetFontFactInstance()->insert_src_font(cur_logfont_);
	CDC* take_place = NULL;
	Task<CDC>* task = NewRunnableMethod(&doc_view_controler, take_place, &LxDcViCtl::usr_insert, cs, len, src_font, cur_color_);
	ExecuteNormalTask(task);
	delete task;
}

void CCLexiView::wrap()
{
	CDC* take_place = NULL;
	Task<CDC>* task = NewRunnableMethod(&doc_view_controler, take_place, &LxDcViCtl::usr_wrap);
	ExecuteNormalTask(task);
	delete task;
}

void CCLexiView::backspace()
{
	CDC* take_place = NULL;
	Task<CDC>* task = NewRunnableMethod(&doc_view_controler, take_place, &LxDcViCtl::usr_backspace);
	ExecuteNormalTask(task);
	delete task;
}


void CCLexiView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (ViewWindow::GetViewWindowInstance()->offset_x != GetScrollPosition().x)
	{
		ViewWindow::GetViewWindowInstance()->offset_x = GetScrollPosition().x;
		CDC* pDC = NULL;
		Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::draw_complete);
		ExecuteNormalTask(task, pDC);
		delete task;
	}

	CScrollView::OnHScroll(nSBCode, nPos, pScrollBar);
}


void CCLexiView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (ViewWindow::GetViewWindowInstance()->offset_y != GetScrollPosition().y)
	{
		ViewWindow::GetViewWindowInstance()->offset_y = GetScrollPosition().y;
		CDC* pDC = NULL;
		Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::draw_complete);
		ExecuteNormalTask(task, pDC);
		delete task;
	}

	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CCLexiView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	CDC* pDC = GetDC();
	switch (nFlags)
	{
	case MK_SHIFT:
		doc_view_controler.modify_mouse_hscroll(pDC, zDelta);
		break;
	case MK_CONTROL:
		break;
	case MK_LBUTTON:
		break;
	case MK_MBUTTON:
		break;
	case MK_RBUTTON:
		break;
	default:
		doc_view_controler.modify_mouse_vscroll(pDC, zDelta);
		break;
	}
	ReleaseDC(pDC);

	return 0;
	//return CScrollView::OnMouseWheel(nFlags, zDelta, pt);
}

void CCLexiView::redraw()
{
	CDC* pDC = NULL;
	Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::draw_complete);
	ExecuteNormalTask(task, pDC);
	delete task;
}


void CCLexiView::OnSetFont()
{
	CFontDialog FontDlg;
	if (FontDlg.DoModal() == IDOK)
	{
		cur_logfont_ = *FontDlg.m_cf.lpLogFont;
		font_index = 0;
		if (doc_view_controler.section_active())
		{
			CDC* pDC = NULL;
			Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::usr_font_change, cur_logfont_);
			ExecuteNormalTask(task, pDC);
			delete task;
		}
	}
}
void CCLexiView::OnSetColor()
{
	CColorDialog cDlg;
	cDlg.m_cc.Flags |= CC_RGBINIT;
	cDlg.m_cc.Flags |= CC_FULLOPEN;
	if (cDlg.DoModal() == IDOK)
	{
		cur_color_ = cDlg.m_cc.rgbResult;
		if (doc_view_controler.section_active())
		{
			CDC* pDC = NULL;
			Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::usr_color_change, cur_color_);
			ExecuteNormalTask(task, pDC);
			delete task;
		}
	}
}
void CCLexiView::set_color(COLORREF color)
{
	cur_color_ = color;
}
void CCLexiView::set_font_index(size_t index)
{
	font_index = index;
}

void CCLexiView::notify_recovery()
{
	CDC* pDC = NULL;
	Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::draw_complete);
	ExecuteNormalTask(task, pDC);
	delete task;
}

void CCLexiView::OnSysCommand(UINT nID, LPARAM lParam)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	CScrollView::OnSysCommand(nID, lParam);
}


void CCLexiView::OnFileSaveAs()
{
	CString filter = L"文本文件(*.txt)|*.txt||";
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		CString str = dlg.GetPathName();
		USES_CONVERSION;
		FILE* file;
		fopen_s(&file, W2A(str), "wb+");
		doc_view_controler.store_stream(file);
		fclose(file);
	}
}


void CCLexiView::OnFileSave()
{
	CString filter = L"文本文件(*.txt)|*.txt||";
	CFileDialog dlg(FALSE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		CString str = dlg.GetPathName();
		USES_CONVERSION;
		FILE* file;
		fopen_s(&file, W2A(str), "wb+");
		doc_view_controler.store_stream(file);
		fclose(file);
	}
}


void CCLexiView::OnFileOpen()
{
	CString filter = L"文本文件(*.txt)|*.txt||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, filter);
	if (dlg.DoModal() == IDOK)
	{
		CString str = dlg.GetPathName();
		USES_CONVERSION;
		FILE* file;
		fopen_s(&file, W2A(str), "rb");
		CDC* pDC = GetDC();
		doc_view_controler.init(pDC, file);
		ViewWindow::GetViewWindowInstance()->offset_x = 0;
		ViewWindow::GetViewWindowInstance()->offset_y = 0;
		doc_view_controler.draw_complete(pDC);
		ReleaseDC(pDC);
		fclose(file);
	}
}

int CCLexiView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	RegisterHotKey(GetSafeHwnd(), 1001, MOD_CONTROL, 'z');
	RegisterHotKey(GetSafeHwnd(), 1002, MOD_CONTROL, 'Z');
	RegisterHotKey(GetSafeHwnd(), 1003, MOD_CONTROL, 'y');
	RegisterHotKey(GetSafeHwnd(), 1004, MOD_CONTROL, 'Y');
	RegisterHotKey(GetSafeHwnd(), 1005, MOD_CONTROL, 'c');
	RegisterHotKey(GetSafeHwnd(), 1006, MOD_CONTROL, 'C');
	RegisterHotKey(GetSafeHwnd(), 1007, MOD_CONTROL, 'x');
	RegisterHotKey(GetSafeHwnd(), 1008, MOD_CONTROL, 'X');
	RegisterHotKey(GetSafeHwnd(), 1009, MOD_CONTROL, 'v');
	RegisterHotKey(GetSafeHwnd(), 1010, MOD_CONTROL, 'V');
	RegisterHotKey(GetSafeHwnd(), 1011, MOD_CONTROL, 'a');
	RegisterHotKey(GetSafeHwnd(), 1012, MOD_CONTROL, 'A');

	return 0;
}

LRESULT CCLexiView::OnHotKey(WPARAM wParam, LPARAM lParam)
{
	CDC* pDC = NULL;
	switch (wParam)
	{
	case 1001:
	case 1002:
	{
		Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::usr_undo);
		ExecuteNormalTask(task, pDC);
		delete task;
		break;
	}
	case 1003:
	case 1004:
	{
		Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::usr_redo);
		ExecuteNormalTask(task, pDC);
		delete task;
		break;
	}
	case 1005:
	case 1006:
	{
		doc_view_controler.usr_copy();
		break;
	}
	case 1007:
	case 1008:
	{
		Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::usr_cut);
		ExecuteNormalTask(task, pDC);
		delete task;
		break;
	}
	case 1009:
	case 1010:
	{
		Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::usr_paste);
		ExecuteNormalTask(task, pDC);
		delete task;
		break;
	}
	case 1011:
	case 1012:
	{
		Task<CDC>* task = NewRunnableMethod(&doc_view_controler, pDC, &LxDcViCtl::usr_select_all);
		ExecuteNormalTask(task, pDC);
		delete task;
		break;
	}
	default:
		break;
	}

	return 0;
}
