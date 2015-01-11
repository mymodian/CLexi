#include "stdafx.h"
#include "LxViewWindow.h"
#include "LxPaper.h"

ViewWindow* ViewWindow::lpViewWindowInstance = nullptr;

void AdjustViewWindow(int width_new, int height_new)
{
	ViewWindow::GetViewWindowInstance()->width = width_new;
	ViewWindow::GetViewWindowInstance()->height = height_new;
	int space_width = width_new - LxPaper::pixel_width;
	ViewWindow::GetViewWindowInstance()->border_width_left = space_width > 0 ? space_width / 2 : 0;
	ViewWindow::GetViewWindowInstance()->border_width_right = space_width > 0 ? space_width - space_width / 2 : 0;
}