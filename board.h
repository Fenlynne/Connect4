#pragma once
#include "mct.h"
#include "Judge.h"
#include "argument.h"
#include <ctime>
//top_[x] 表示 x 列最低的空的位置
class BoardState : public MCTState
{
public:
	us *state_, *top_;

	int count;

	bool terminate;

	int next_child_state_;

	bool player_;

	Point move_, lastMove_;

	static int BoardWidth_, BoardHeight_, noX_, noY_;

	static int **board_;

	void Print() override
	{
		TransToBoard();
		for (int i = 0; i < BoardHeight_; ++i)
		{
			for (int j = 0; j < BoardWidth_; ++j)
				if (board_[i][j] == 0)
					_cprintf(". ");
				else if (board_[i][j] == 1)
					_cprintf("A ");
				else if (board_[i][j] == 2)
					_cprintf("B ");
				else
					_cprintf("X ");

			_cprintf("\n");
		}
		_cprintf("\n");
	}

	BoardState(bool player);

	BoardState(const BoardState &pre_state, Point location);

	virtual ~BoardState();

	void TransToBoard();

	Point GetMove();

	Point GetLastMove();
	
	void SetLastMove(Point move);

	Point Put();

	Point PutRandomly();

	MCTState *NextChildState();

	int DefaultPolicy() override;
};

class BoardNode : public MCTNode
{
public:
	BoardNode(MCTState *state, MCTNode *parent) : MCTNode(state, parent) {}
	MCTNode *Expand() override;
};

class BoardUCT : public MCT
{
	float UCB(MCTNode *parent, MCTNode *child);
	MCTNode *BestChild(MCTNode *parent);
	void BackUp(MCTNode *node, int value);

public:
	static DWORD start_time;
	BoardUCT(int player) : MCT(new BoardNode(new BoardState(player), nullptr)) {}
	MCTNode *TreePolicy() override;
	void Step(Point location);

	void print() {
		root_->GetState()->Print();
	}
	Point MCTS();
};