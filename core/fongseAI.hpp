#ifndef FONGSEAI_HPP
#define FONGSEAI_HPP

#include "OthelloAI.hpp"

namespace fongse
{
	class FongseAI : public OthelloAI
	{
		public:
			virtual std::pair<int,int> chooseMove(const OthelloGameState& state);
			int search(OthelloGameState* state, int depth, bool myTurn, int alpha, int beta);
			int evaluateMove(OthelloGameState* state);
		private:
			int bestX, bestY;
	};
}
#endif