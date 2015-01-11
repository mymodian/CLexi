#include "stdafx.h"
#include "LxCommon.h"

void store_stream_int(FILE* file, int n)
{
	unsigned char tb[4];
	tb[0] = n & 0x000000ff;
	tb[1] = (n >> 8) & 0x000000ff;
	tb[2] = (n >> 16) & 0x000000ff;
	tb[3] = (n >> 24) & 0x000000ff;
	fwrite(tb, sizeof(unsigned char), 4, file);
}
int read_stream_int(FILE* file)
{
	unsigned char tb[4];
	fread(tb, sizeof(unsigned char), 4, file);
	int rt = tb[3];
	rt <<= 8; rt += tb[2];
	rt <<= 8; rt += tb[1];
	rt <<= 8; rt += tb[0];
	return rt;
}