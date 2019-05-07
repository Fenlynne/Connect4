/********************************************************
*	Strategy.h : 策略接口文件                           *
*	张永锋                                              *
*	zhangyf07@gmail.com                                 *
*	2010.8                                              *
*********************************************************/

#ifndef STRATEGY_H_
#define	STRATEGY_H_

#include "Point.h"
#include "board.h"
extern "C" __declspec(dllexport) Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY);

extern "C" __declspec(dllexport) void clearPoint(Point* p);

void clearArray(int M, int N, int** board);

int BoardState::noX_, BoardState::noY_, BoardState::BoardWidth_, BoardState::BoardHeight_, **BoardState::board_;
DWORD BoardUCT::start_time;
BoardUCT* uct;

void InitGame(int M, int N, int noX, int noY);
bool Start(const int* board, int M, int N);
/*
	添加你自己的辅助函数
*/


#endif