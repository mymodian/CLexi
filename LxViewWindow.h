#ifndef __LEXI_VIEW_WINDOW_H
#define __LEXI_VIEW_WINDOW_H

class ViewWindow
{
public:
	int width;
	int height;
	int offset_x;
	int offset_y;
	int border_width_left;
	int border_width_right;
	int border_height;
	COLORREF view_back_color_;
	//属性均在实际情况中设置
public:
	static ViewWindow* GetViewWindowInstance()
	{
		if (lpViewWindowInstance == nullptr)
			lpViewWindowInstance = new ViewWindow();
		return lpViewWindowInstance;
	}
	void set_view_back_color(COLORREF view_back_color)
	{
		view_back_color_ = view_back_color;
	}
	COLORREF get_view_back_color()
	{
		return view_back_color_;
	}
	int get_top_pos()
	{
		return offset_y;
	}
	int get_bottom_pos()
	{
		return offset_y + height;
	}
private:
	static ViewWindow* lpViewWindowInstance;
private:
	ViewWindow() : border_height(20), offset_x(0), offset_y(0), view_back_color_(RGB(0,100,100)) {}
	ViewWindow(const ViewWindow&) = delete;
	ViewWindow& operator=(const ViewWindow&) = delete;
};

void AdjustViewWindow(int width_new, int height_new);

#endif