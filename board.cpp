#include "board.h"
#include "fast_math.hpp"
BoardState::BoardState(bool player) : player_(player), next_child_state_(0), terminate(0)
{
	count = 0;
	state_ = new us[BoardWidth_];
	top_ = new us[BoardWidth_];
	memset(state_, 0, BoardWidth_ * sizeof(us));
	memset(top_, 0, BoardWidth_ * sizeof(us));

	if (noY_ == 0) top_[noX_] = 1;

	BoardState::board_ = new int *[BoardState::BoardHeight_];

	for (int i = 0; i < BoardState::BoardHeight_; i++)
		board_[i] = new int[BoardWidth_];

	srand((unsigned int)time(0));
}

BoardState::BoardState(const BoardState &pre_state, Point location) : next_child_state_(0), move_(location), player_(!pre_state.player_), terminate(0)
{
	state_ = new us[BoardWidth_];
	top_ = new us[BoardWidth_];
	memcpy(state_, pre_state.state_, BoardWidth_ * sizeof(us));
	memcpy(top_, pre_state.top_, BoardWidth_ * sizeof(us));

	int x = location.first;
	int y = location.second;

	state_[x] |= player_ ? 0 : (1 << y);
	top_[x] = us((x == noX_ && y + 1 == noY_) ? (y + 2) : (y + 1));

	//  judge terminal state

	TransToBoard();
	int& cnt = count;
	cnt = 0;

	//GameOver
	if ((!player_ && machineWin(BoardHeight_-y-1, x, BoardHeight_, BoardWidth_, board_))
		|| (player_ && userWin(BoardHeight_ - y - 1, x, BoardHeight_, BoardWidth_, board_))) {
		next_child_state_ = BoardWidth_;
	}
	else {
		//jugde whether the player can win in one step
		for (int i = 0; i < BoardWidth_; ++i) {
			int j = BoardHeight_ - top_[i] - 1;
			if (j < 0) continue;
			board_[j][i] = (player_)+1;
			if ((player_ && machineWin(j, i, BoardHeight_, BoardWidth_, board_))
				|| (!player_ && userWin(j, i, BoardHeight_, BoardWidth_, board_))) {
				next_child_state_ = i;
				terminate = 1;
			}
			board_[j][i] = 0;
			if (terminate) break;
		}
		//jugde whether the player will lose in two steps
		if (!terminate) {
			for (int i = 0; i < BoardWidth_; ++i) {
				int j = BoardHeight_ - top_[i] - 1;
				if (j < 0) continue;
				board_[j][i] = (!player_) + 1;

				if ((!player_ && machineWin(j, i, BoardHeight_, BoardWidth_, board_))
					|| (player_ && userWin(j, i, BoardHeight_, BoardWidth_, board_)))
				{
					++cnt;
					int d = 1;
					if (i == noX_ && j - d == noY_) ++d;

					if (j - d >= 0) {
						board_[j][i] = (player_)+1;
						board_[j - d][i] = (!player_) + 1;

						if ((!player_ && machineWin(j - d, i, BoardHeight_, BoardWidth_, board_))
							|| (player_ && userWin(j - d, i, BoardHeight_, BoardWidth_, board_)))
							++cnt;

						board_[j][i] = (!player_) + 1;
						board_[j - d][i] = 0;
					}
				}
				board_[j][i] = 0;
			}

			if (cnt >= 2)
				terminate = 1;

			while (next_child_state_ < BoardWidth_ && top_[next_child_state_] >= BoardHeight_)
				++next_child_state_;
		}
	}
}

BoardState::~BoardState() {
	delete[] top_;
	delete[] state_;
}

Point BoardState::Put()
{
	Point &&location = std::make_pair(next_child_state_, (int)top_[next_child_state_]);

	if (!terminate) {
		do {
			++next_child_state_;
		} while (next_child_state_ < BoardWidth_ && top_[next_child_state_] >= BoardHeight_);
	}
	else {
		next_child_state_ = BoardWidth_;
	}
	return location;
}

Point BoardState::GetMove() { return move_; }
Point BoardState::GetLastMove() { return lastMove_; }
void BoardState::SetLastMove(Point move) { lastMove_ = move; }
Point BoardState::PutRandomly()
{
	int ccnt = 0;
	for (int i = 0; i < BoardWidth_; ++i)
		if (top_[i] < BoardHeight_)
			ccnt++;
	//棋盘已满
	if (ccnt == 0)
		return std::make_pair(-1, -1);
	//否则随机选取可下棋位置

	int cnt = rand() % ccnt;

	for (int i = 0; i < BoardWidth_; ++i) {
		if (top_[i] < BoardHeight_) {
			if (cnt == 0)
				return std::make_pair(i, top_[i]);
			else
				--cnt;
		}
	}
	return std::make_pair(-1, -1);
}


MCTState *BoardState::NextChildState()
{
	return next_child_state_ == BoardWidth_ ? nullptr : new BoardState(*this, Put());
}

void BoardState::TransToBoard()
{
	for (int i = 0; i < BoardHeight_; i++)
		memset(board_[i], 0, BoardWidth_ * sizeof(int));

	for (int j = 0; j < BoardWidth_; j++)
		for (int i = 0; i < top_[j]; i++)
			if (!(j == noX_ && i == noY_))
				board_[BoardHeight_ - i - 1][j] = (((1 << i) & state_[j]) > 0) + 1;
}

int BoardState::DefaultPolicy()
{
	//when a player finds that the state is terminal, that means he has lost the game
	if (next_child_state_ == BoardWidth_)
	{
		return -1;
	}

	bool player = player_;
	Point location;
	TransToBoard();

	us *top = new us[BoardWidth_];
	//make a back-up of top
	memcpy(top, top_, BoardWidth_ * sizeof(us));

	//if the loop stops when the board is full, the result must be 'Tie'
	while ((location = PutRandomly()).second != -1)
	{
		int x = location.first, y = location.second;
		top_[x] = us((x == noX_ && y + 1 == noY_) ? (y + 2) : (y + 1));
		board_[BoardHeight_ - y - 1][x] = player + 1;
		if ((player && machineWin(BoardHeight_ - y - 1, x, BoardHeight_, BoardWidth_, board_)) || (!player && userWin(BoardHeight_ - y - 1, x, BoardHeight_, BoardWidth_, board_)))
		{
			break;
		}
		player = 1 - player;
	}

	//recover the top
	memcpy(top_, top, BoardWidth_ * sizeof(us));
	delete[] top;
	//  Tie / Win / Lose
	return location.second == -1 ? 0 : (player == player_ ? 1 : -1);
}

MCTNode *BoardNode::Expand()
{
	//in-order expand
	MCTState *new_state = state_->NextChildState();
	//unexpandable: state＿　is　a terminal state, so it has no child state
	if (new_state == nullptr)
		return nullptr;
	MCTNode *new_node = new BoardNode(new_state, this);
	//expandable
	if (first_child_ == nullptr)
		first_child_ = new_node;
	else
	{
		new_node->SetNextSibling(first_child_);
		SetFirstChild(new_node);
	}
	return new_node;
}

float BoardUCT::UCB(MCTNode *parent, MCTNode *child)
{
	int &&N = child->GetVisitTimes();
	float &&Q = -child->GetValue();
	return Q / N + C * FastSqrt(2 * FastLog(parent->GetVisitTimes()) / N);
}

MCTNode *BoardUCT::BestChild(MCTNode *parent)
{
	float max_score = -INF;
	MCTNode *best_child = nullptr;
	for (MCTNode *child = parent->GetFirstChild(); child != nullptr; child = child->GetNextSibling())
	{
		float &&score = UCB(parent, child);
		if (score > max_score)
		{
			max_score = score;
			best_child = child;
		}
	}
	return best_child;
}

void BoardUCT::BackUp(MCTNode *node, int value)
{
	while (node != nullptr)
	{
		node->AddValue(value);
		value = -value;
		node = node->GetParent();
	}
}

MCTNode *BoardUCT::TreePolicy()
{
	MCTNode *cur_node = root_, *new_node;

	while ((new_node = cur_node->Expand()) == nullptr && cur_node->GetFirstChild() != nullptr)
	{
		cur_node = BestChild(cur_node);
	}

	return cur_node->GetFirstChild() == nullptr ? cur_node : new_node;
}

void BoardUCT::Step(Point location)
{
	//the first step of the player && the second step of the game
	MCTNode *new_root = root_->GetFirstChild();
	if (new_root == nullptr)
	{
		while ((new_root = root_->Expand()) != nullptr)
		{
			if (((BoardState *)new_root->GetState())->GetMove() == location)
			{
				break;
			}
		}
	}
	//otherwise, choose the situation from children
	else
	{
		while (((BoardState *)new_root->GetState())->GetMove() != location)
		{
			new_root = new_root->GetNextSibling();
			if (new_root == nullptr) {
				new_root = new BoardNode(new BoardState(*((BoardState*)(root_->GetState())), location), root_);
				break;
			}
		}
	}
	//recycle space
	MCTNode *child = root_->GetFirstChild(), *next;
	while (child != nullptr)
	{
		next = child->GetNextSibling();
		if (child != new_root)
			delete child;
		child = next;
	}
	root_->SetFirstChild(new_root);
	new_root->SetNextSibling(nullptr);
	root_ = new_root;
}

Point BoardUCT::MCTS()
{
	int clock = 0;
	do
	{
		MCTNode *new_node = TreePolicy();
		if (((BoardState*)(new_node->GetState()))->terminate) {
			MCTNode* nd;
			while (nd = new_node->Expand()) {
				new_node = nd;
			}
		}
		BackUp(new_node, new_node->GetState()->DefaultPolicy());
	} while (++clock % 1000 != 0 || ::GetTickCount() - start_time < TIME_LIMIT);

	int max_visit_times = -INF;
	MCTNode *best_child = nullptr;
	for (MCTNode *child = root_->GetFirstChild(); child != nullptr; child = child->GetNextSibling())
	{
		if (child->GetVisitTimes() > max_visit_times)
		{
			max_visit_times = child->GetVisitTimes();
			best_child = child;
		}
	}

	Point&& move = ((BoardState *)best_child->GetState())->GetMove();
	Step(move);
	return move;
}
