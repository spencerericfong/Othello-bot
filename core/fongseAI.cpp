#include <ics46/factory/DynamicFactory.hpp>
#include "fongseAI.hpp"
#include <vector>

ICS46_DYNAMIC_FACTORY_REGISTER(OthelloAI, fongse::FongseAI, "fongseAI (Required)");

std::pair<int, int> fongse::FongseAI::chooseMove(const OthelloGameState& state)
{
	std::unique_ptr<OthelloGameState> stateCopy = state.clone();
	search(stateCopy.get(), 6, true, -1000, 1000);
	return std::pair<int,int>(bestX, bestY);
}

int fongse::FongseAI::search(OthelloGameState* state, int depth, bool myTurn, int alpha, int beta)
{
	if (depth == 0)
	{
		return evaluateMove(state);		//base case, reached max depth so evaluate the max depth board
	}

	else
	{
		//Finds all valid moves for the current state and stores in a vector
		std::vector<std::pair<int,int>> validMoves;
		for (int i = 0; i < state->board().width(); i++)
		{
			for (int j = 0; j < state->board().height(); j++)
			{
				if (state->isValidMove(i, j))
				{
					validMoves.push_back(std::pair<int,int>(i, j));
				}
			}
		}
		//If there are no valid moves, evaluate the current state without recursing
		if (validMoves.size() == 0)
		{
			return evaluateMove(state);
		}

		//If it is the maximizing player's turn, find the maximum value move
		if (myTurn)
		{
			int bestValue = -1000;
			for (int i = 0; i < validMoves.size(); i++)
			{
				std::unique_ptr<OthelloGameState> newState = state->clone();
				newState->makeMove(validMoves[i].first, validMoves[i].second);
				int value = search(newState.get(), depth - 1, !myTurn, alpha, beta);
				if (value > bestValue)
				{
					bestValue = value;
				}
				if (bestValue > alpha)
				{
					alpha = bestValue;
					if (depth == 6)
					{
						bestX = validMoves[i].first;
						bestY = validMoves[i].second;
					}
				}
				if (beta <= alpha)
					break;
			}

			return bestValue;
		}

		//If it is the minimizing player's turn, find the minimum value move
		else
		{
			int bestValue = 1000;
			for (int i = 0; i < validMoves.size(); i++)
			{
				std::unique_ptr<OthelloGameState> newState = state->clone();
				newState->makeMove(validMoves[i].first, validMoves[i].second);
				int value = search(newState.get(), depth - 1, !myTurn, alpha, beta);
				if (value < bestValue)
				{
					bestValue = value;
				}
				if (bestValue < beta)
				{
					beta = bestValue;
				}
				if (beta <= alpha)
					break;
			}

			return bestValue;
		}
	}
}

int fongse::FongseAI::evaluateMove(OthelloGameState* state)
{
	//If maximizing player is black, return black score - white score, after assessing risk regions
	if (state->isBlackTurn())
	{
		int score = state->blackScore() - state->whiteScore();

		//Adds big bonus for corner tiles
		if (state->board().cellAt(0,0) == OthelloCell::black)
			score += 10;
		if (state->board().cellAt(0,state->board().width()-1) == OthelloCell::black)
			score += 10;
		if (state->board().cellAt(state->board().height()-1,0) == OthelloCell::black)
			score += 10;
		if (state->board().cellAt(state->board().width()-1,state->board().height()-1) == OthelloCell::black)
			score += 10;

		//Adds small bonus for middle outer edge tiles
		if (state->board().cellAt(2,0) == OthelloCell::black || state->board().cellAt(3,0) == OthelloCell::black || 
				state->board().cellAt(4,0) == OthelloCell::black || state->board().cellAt(5,0) == OthelloCell::black)
			score += 5;
		if (state->board().cellAt(2,state->board().height()-1) == OthelloCell::black || state->board().cellAt(3,state->board().height()-1) == OthelloCell::black || 
				state->board().cellAt(4,state->board().height()-1) == OthelloCell::black || state->board().cellAt(5,state->board().height()-1) == OthelloCell::black)
			score += 5;
		if (state->board().cellAt(0,2) == OthelloCell::black || state->board().cellAt(0,3) == OthelloCell::black || 
				state->board().cellAt(0,4) == OthelloCell::black || state->board().cellAt(0,5) == OthelloCell::black)
			score += 5;
		if (state->board().cellAt(state->board().width()-1,2) == OthelloCell::black || state->board().cellAt(state->board().width()-1,3) == OthelloCell::black || 
				state->board().cellAt(state->board().width()-1,4) == OthelloCell::black || state->board().cellAt(state->board().width()-1,5) == OthelloCell::black)
			score += 5;

		//Adds big penalty for tiles surrounding corners
		if (state->board().cellAt(0,1) == OthelloCell::black || state->board().cellAt(1,1) == OthelloCell::black || state->board().cellAt(1,0) == OthelloCell::black)
			score -= 10;
		if (state->board().cellAt(state->board().width()-2,0) == OthelloCell::black || state->board().cellAt(state->board().width()-2,1) == OthelloCell::black || 
				state->board().cellAt(state->board().width()-1, 1) == OthelloCell::black)
			score -= 10;
		if (state->board().cellAt(0,state->board().height()-2) == OthelloCell::black || state->board().cellAt(1,state->board().height()-2) == OthelloCell::black || 
				state->board().cellAt(1,state->board().height()-1) == OthelloCell::black)
			score -= 10;
		if (state->board().cellAt(state->board().width()-2,state->board().height()-2) == OthelloCell::black || state->board().cellAt(state->board().width()-1,state->board().height()-2) == OthelloCell::black || 
				state->board().cellAt(state->board().width()-2,state->board().height()-1) == OthelloCell::black)
			score -= 10;

		//Adds small penalty for middle inner edge tiles
		if (state->board().cellAt(1,2) == OthelloCell::black || state->board().cellAt(1,3) == OthelloCell::black || state->board().cellAt(1,4) == OthelloCell::black ||
				state->board().cellAt(1,5) == OthelloCell::black)
			score -= 5;
		if (state->board().cellAt(2,1) == OthelloCell::black || state->board().cellAt(3,1) == OthelloCell::black || state->board().cellAt(4,1) == OthelloCell::black ||
				state->board().cellAt(5,1) == OthelloCell::black)
			score -= 5;
		if (state->board().cellAt(2,6) == OthelloCell::black || state->board().cellAt(3,6) == OthelloCell::black || state->board().cellAt(4,6) == OthelloCell::black ||
				state->board().cellAt(5,6) == OthelloCell::black)
			score -= 5;
		if (state->board().cellAt(6,2) == OthelloCell::black || state->board().cellAt(6,3) == OthelloCell::black || state->board().cellAt(6,4) == OthelloCell::black ||
				state->board().cellAt(6,5) == OthelloCell::black)
			score -= 5;

		return score;
	}

	//If maximizing player is white, return white score - black score, after assessing risk regions
	else
	{
		int score = state->whiteScore() - state->blackScore();

		//Adds big bonus for corner tiles
		if (state->board().cellAt(0,0) == OthelloCell::white)
			score += 10;
		if (state->board().cellAt(0,state->board().width()-1) == OthelloCell::white)
			score += 10;
		if (state->board().cellAt(state->board().height()-1,0) == OthelloCell::white)
			score += 10;
		if (state->board().cellAt(state->board().width()-1,state->board().height()-1) == OthelloCell::white)
			score += 10;

		//Adds small bonus for middle outer edge tiles
		if (state->board().cellAt(2,0) == OthelloCell::white || state->board().cellAt(3,0) == OthelloCell::white || 
				state->board().cellAt(4,0) == OthelloCell::white || state->board().cellAt(5,0) == OthelloCell::white)
			score += 5;
		if (state->board().cellAt(2,state->board().height()-1) == OthelloCell::white || state->board().cellAt(3,state->board().height()-1) == OthelloCell::white || 
				state->board().cellAt(4,state->board().height()-1) == OthelloCell::white || state->board().cellAt(5,state->board().height()-1) == OthelloCell::white)
			score += 5;
		if (state->board().cellAt(0,2) == OthelloCell::white || state->board().cellAt(0,3) == OthelloCell::white || 
				state->board().cellAt(0,4) == OthelloCell::white || state->board().cellAt(0,5) == OthelloCell::white)
			score += 5;
		if (state->board().cellAt(state->board().width()-1,2) == OthelloCell::white || state->board().cellAt(state->board().width()-1,3) == OthelloCell::white || 
				state->board().cellAt(state->board().width()-1,4) == OthelloCell::white || state->board().cellAt(state->board().width()-1,5) == OthelloCell::white)
			score +=5;

		//Adds big penalty for tiles surrounding corners
		if (state->board().cellAt(0,1) == OthelloCell::white || state->board().cellAt(1,1) == OthelloCell::white || state->board().cellAt(1,0) == OthelloCell::white)
			score -= 10;
		if (state->board().cellAt(state->board().width()-2,0) == OthelloCell::white || state->board().cellAt(state->board().width()-2,1) == OthelloCell::white || 
				state->board().cellAt(state->board().width()-1, 1) == OthelloCell::white)
			score -= 10;
		if (state->board().cellAt(0,state->board().height()-2) == OthelloCell::white || state->board().cellAt(1,state->board().height()-2) == OthelloCell::white || 
				state->board().cellAt(1,state->board().height()-1) == OthelloCell::white)
			score -= 10;
		if (state->board().cellAt(state->board().width()-2,state->board().height()-2) == OthelloCell::white || state->board().cellAt(state->board().width()-1,state->board().height()-2) == OthelloCell::white || 
				state->board().cellAt(state->board().width()-2,state->board().height()-1) == OthelloCell::white)
			score -= 10;

		//Adds small penalty for middle inner edge tiles
		if (state->board().cellAt(1,2) == OthelloCell::white || state->board().cellAt(1,3) == OthelloCell::white || state->board().cellAt(1,4) == OthelloCell::white ||
				state->board().cellAt(1,5) == OthelloCell::white)
			score -= 5;
		if (state->board().cellAt(2,1) == OthelloCell::white || state->board().cellAt(3,1) == OthelloCell::white || state->board().cellAt(4,1) == OthelloCell::white ||
				state->board().cellAt(5,1) == OthelloCell::white)
			score -= 5;
		if (state->board().cellAt(2,6) == OthelloCell::white || state->board().cellAt(3,6) == OthelloCell::white || state->board().cellAt(4,6) == OthelloCell::white ||
				state->board().cellAt(5,6) == OthelloCell::white)
			score -= 5;
		if (state->board().cellAt(6,2) == OthelloCell::white || state->board().cellAt(6,3) == OthelloCell::white || state->board().cellAt(6,4) == OthelloCell::white ||
				state->board().cellAt(6,5) == OthelloCell::white)
			score -= 5;

		return score;
	}

	return 0;
}