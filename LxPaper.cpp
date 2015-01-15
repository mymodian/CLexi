#include "stdafx.h"
#include "LxPaper.h"

int LxPaper::pixel_width = 595;
int LxPaper::pixel_height = 250;
//int LxPaper::pixel_height = 842;

COLORREF LxPaper::paper_back_color = RGB(100, 100, 100);

int LxPaper::margin_mode = NORMAL_MARGIN;
int LxPaper::left_margin = 90;
int LxPaper::right_margin = 90;
int LxPaper::top_margin = 72;
int LxPaper::bottom_margin = 72;