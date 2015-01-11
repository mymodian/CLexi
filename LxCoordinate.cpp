#include "stdafx.h"
#include "LxCoordinate.h"

int index_to_cursor(int index)
{
	return index + 1;
}
int cursor_to_index(int cursor)
{
	return cursor - 1;
}