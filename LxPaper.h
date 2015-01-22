#include <Windows.h>
#ifndef __LEXI_PAPER_H
#define __LEXI_PAPER_H

#define NARROW_MARGIN			0
#define NORMAL_MARGIN			1
#define MODERATE_MARGIN		2
#define WIDE_MARGIN				3

#define MIN_PAPER_WIDTH			144*2+100
#define MIN_PAPER_HEIGHT		72*2+100

struct LxPaper
{
	/*static float width;
	static float height;*/
	static int pixel_width;
	static int pixel_height;

	static int left_margin;
	static int right_margin;
	static int top_margin;
	static int bottom_margin;

	static int margin_mode;
	static COLORREF paper_back_color;

	static bool set_back_color(COLORREF back_color)
	{
		if( paper_back_color == back_color )
			return false;
		paper_back_color = back_color;
		return true;
	}

	static bool set_to_a4()
	{
		if( LxPaper::pixel_width == 595 && LxPaper::pixel_height == 842 )
			return false;
		LxPaper::pixel_width		=	595;
		LxPaper::pixel_height	=	842;
		return true;
	}
	static bool set_to_custom(int pixel_width, int pixel_height)
	{
		if( LxPaper::pixel_width == pixel_width && LxPaper::pixel_height == pixel_height )
			return false;
		LxPaper::pixel_width = pixel_width < MIN_PAPER_WIDTH ? MIN_PAPER_WIDTH : pixel_width;
		LxPaper::pixel_height = pixel_height < MIN_PAPER_HEIGHT ? MIN_PAPER_HEIGHT : pixel_height;
		return true;
	}
	static bool set_narrow_margin()
	{
		if( margin_mode == NARROW_MARGIN )
			return false;
		margin_mode = NARROW_MARGIN;
		LxPaper::left_margin			=	36;
		LxPaper::right_margin		=	36;
		LxPaper::top_margin			=	36;
		LxPaper::bottom_margin	=	36;
		return true;
	}
	static bool set_normal_margin()
	{
		if( margin_mode == NORMAL_MARGIN )
			return false;
		margin_mode = NORMAL_MARGIN;
		LxPaper::left_margin			=	90;
		LxPaper::right_margin		=	90;
		LxPaper::top_margin			=	72;
		LxPaper::bottom_margin	=	72;
		return true;
	}
	static bool set_moderate_margin()
	{
		if( margin_mode == MODERATE_MARGIN )
			return false;
		margin_mode = MODERATE_MARGIN;
		LxPaper::left_margin			=	54;
		LxPaper::right_margin		=	54;
		LxPaper::top_margin			=	72;
		LxPaper::bottom_margin	=	72;
		return true;
	}
	static bool set_wide_margin()
	{
		if( margin_mode == WIDE_MARGIN )
			return false;
		margin_mode = WIDE_MARGIN;
		LxPaper::left_margin			=	144;
		LxPaper::right_margin		=	144;
		LxPaper::top_margin			=	72;
		LxPaper::bottom_margin	=	72;
		return true;
	}
	//A0 841×1189	A1 594×841		A2 420×594		A3 297×420		A4 210×297  thisPC 595×842xxx  770×1090
};

#endif
//A4规格：210*297毫米
//获取屏幕的分辨率和物理尺寸
//int cx = GetSystemMetrics(SM_CXSCREEN);
//int cy = GetSystemMetrics(SM_CYSCREEN);
//
//int nScreenWidth, nScreenHeight;
//HDC hdcScreen = GetDC(NULL);			//获取屏幕的HDC
//nScreenWidth = GetDeviceCaps(hdcScreen, HORZSIZE);
//nScreenHeight = GetDeviceCaps(hdcScreen, VERTSIZE);