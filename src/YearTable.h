#ifndef CN_HUST_LYH_YEAR_TABLE_H
#define CN_HUST_LYH_YEAR_TABLE_H

#include "Util_Arr2D.h"
#include "Solver.h"

namespace hust {

class Solver;
struct TwoNodeMove;

class YearTable
{
public:

	util::Array2D<int> table;
	Input* input;
	int squIter = 1;

	LL getYearOfMove(Solver* solver, TwoNodeMove t);
	bool updateYearTable(Solver* solver, TwoNodeMove t);
	void squIterGrowUp(int step);

	YearTable(Input* input);
	~YearTable();

private:

};
}


#endif // CN_HUST_LYH_YEAR_TABLE_H