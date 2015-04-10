#include "stdafx.h"
#include <iostream>
using namespace std;

#ifndef __LEXI_COMMON_H
#define __LEXI_COMMON_H

typedef enum
{
	COMPOSE_ALGO_SIMPLE = 0,
	COMPOSE_ALGO_LEFT,
	COMPOSE_ALGO_RIGHT,
	COMPOSE_ALGO_CENTER
}ComposeAlgoType;

void store_stream_int(FILE* file, int n);
int read_stream_int(FILE* file);

#endif