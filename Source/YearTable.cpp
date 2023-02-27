
#include <vector>

#include "YearTable.h"

namespace hust {

YearTable::YearTable(Input* input):input(input) {
	table = util::Array2D<int>(input->customerNumer + 1, input->customerNumer + 1, 0);
}

YearTable::~YearTable(){
}

LL YearTable::getYearOfMove(Solver* solver, TwoNodeMove t) {

	int v = t.v;
	int w = t.w;

	auto& customers = solver->customers;
	LL sumYear = 0;

	if (t.kind == 0) {
		//twoOptStarOpenvv_
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		sumYear = (table[w][v] + table[v_][wj]) / 2;
	}
	else if (t.kind == 1) {
		//twoOptStarOpenvvj
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		sumYear = (table[v][w] + table[w_][vj]) / 2;
	}
	else if (t.kind == 2) {
		//outOnevToww_
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		sumYear = (table[v_][vj] + table[w_][v] + table[v][w]) / 3;

	}
	else if (t.kind == 3) {
		//outOnevTowwj
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		sumYear = (table[v_][vj] + table[w][v] + table[v][wj]) / 3;
	}
	else if (t.kind == 4) {
		//inOnevv_
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;

		sumYear = (table[w_][wj] + table[v_][w] + table[w][v]) / 3;
	}
	else if (t.kind == 5) {
		//inOnevvj
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;

		sumYear = (table[w_][wj] + table[v][w] + table[w][vj]) / 3;
	}
	else if (t.kind == 6) {
		//swapOneOnevw_
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int w__ = customers[w_].prev > input->customerNumer ? 0 : customers[w_].prev;

		int rvId = customers[v].routeId;
		int rwId = customers[w].routeId;

		if (rvId == rwId) {

			if (v == w__) {

				sumYear = (table[v_][w_] + table[w_][v]
					+ table[v][w]) / 3;
			}
			else {
				// v- v vj | w-- w- w
				sumYear = (table[w__][v] + table[v][w]
					+ table[v_][w_] + table[w_][vj]) / 4;

			}

		}
		else {

			sumYear = (table[w__][v] + table[v][w]
				+ table[v_][w_] + table[w_][vj]) / 4;
		}
	}
	else if (t.kind == 7) {
		//swapOneOnevwj
		//int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int wjj = customers[wj].next > input->customerNumer ? 0 : customers[wj].next;

		int rvId = customers[v].routeId;
		int rwId = customers[w].routeId;

		if (rvId == rwId) {

			if (v == wj) {
				return false;
			}
			else if (v == wjj) {

				sumYear = (table[w][v] + table[v][wj]
					+ table[wj][vj]) / 3;
			}
			else {
				// v- v vj |  w wj wjj
				sumYear = (table[w][v] + table[v][wjj]
					+ table[v_][wj] + table[wj][vj]) / 4;
			}

		}
		else {

			sumYear = (table[w][v] + table[v][wjj]
				+ table[v_][wj] + table[wj][vj]) / 4;
		}
	}
	else if (t.kind == 8) {
		//swapOneOnevw
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;

		int rvId = customers[v].routeId;
		int rwId = customers[w].routeId;

		if (rvId == rwId) {

			if (v_ == w) {

				sumYear = (table[w_][v] + table[w][vj]
					+ table[v][w]) / 3;
			}
			else if (w_ == v) {

				sumYear = (table[v_][w] + table[w][v]
					+ table[v][wj]) / 3;
			}
			else {
				// v- v vj |  w- w wj
				sumYear = (table[w_][v] + table[v][wj]
					+ table[v_][w] + table[w][vj]) / 4;
			}

		}
		else {

			sumYear = (table[w_][v] + table[v][wj]
				+ table[v_][w] + table[w][vj]) / 4;
		}

	}
	else if (t.kind == 9) {

		//swapThreeTwovvjvjjwwj(v, w); 三换二 v v+ v++ | w w+

		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int vjj = customers[vj].next > input->customerNumer ? 0 : customers[vj].next;
		int v3j = customers[vjj].next > input->customerNumer ? 0 : customers[vjj].next;

		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int wjj = customers[wj].next > input->customerNumer ? 0 : customers[wj].next;

		sumYear = (table[v][w_] + table[vjj][wjj]
			+ table[v_][w] + table[wj][v3j]) / 4;

	}
	else if (t.kind == 10) {

		//swapThreeOnevvjvjjw(v, w); 三换一 v v + v++ | w

		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int vjj = customers[vj].next > input->customerNumer ? 0 : customers[vj].next;
		int v3j = customers[vjj].next > input->customerNumer ? 0 : customers[vjj].next;

		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;

		sumYear = (table[v][w_] + table[vjj][wj]
			+ table[v_][w] + table[w][v3j]) / 4;

	}
	else if (t.kind == 11) {
		//swapTwoOnevvjw(v, w); 二换一 v v +  | w

		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int vjj = customers[vj].next > input->customerNumer ? 0 : customers[vj].next;

		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;

		sumYear = (table[v][w_] + table[vj][wj]
			+ table[v_][w] + table[w][vjj]) / 4;
	}
	else if (t.kind == 12) {

		//swapOneTwovwwj(v, w); 一换二 v  | w w+

		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;

		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int wjj = customers[wj].next > input->customerNumer ? 0 : customers[wj].next;

		sumYear = (table[w_][v] + table[v][wjj]
			+ table[v_][w] + table[wj][vj]) / 4;

	}
	else if (t.kind == 13) {

		//outTwovvjTowwj(v, w); 扔两个 v v+  | w w+

		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		//int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		//int wjj = customers[wj].next > input->customerNumer ? 0 : customers[wj].next;

		sumYear = (table[v][w] + table[vj][wj] + table[v][vj]) / 3;
	}
	else if (t.kind == 14) {

		//outTwovv_Toww_  | w-  v- v w
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int v__ = customers[v_].prev > input->customerNumer ? 0 : customers[v_].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		sumYear = (table[w_][v_] + table[v][w] + table[v__][vj]) / 3;
	}
	else if (t.kind == 15) {

		//reverse [v,w]
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;

		sumYear = (table[v][wj] + table[w][v_]) / 2;
	}
	//else if (t.kind == 16) {
	//	for (int i = 0; i < t.ve.size(); i += 2) {
	//		int v = t.ve[i];
	//		int w = t.ve[(i + 3) % t.ve.size()];
	//		sumYear += table[v][w];
	//	}
	//	sumYear = sumYear / t.ve.size() * 2;
	//}
	else {
		Logger::ERROR("get year of none");
	}
	return sumYear;
}

bool YearTable::updateYearTable(Solver* solver,TwoNodeMove t) {
	
	if (iter * 10 > IntInf) {
		iter = 1;
		for (int i = 0; i < table.size1(); ++i) {
			for (int j = 0; j < table.size2(); ++j) {
				table[i][j] = 1;
			}
		}
	}

	int v = t.v;
	int w = t.w;
	auto& customers = solver->customers;

	auto random = &solver->input->random;
	auto aps = solver->aps;

	if (t.kind == 0) {
		//twoOptStarOpenvv_
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;

		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 1) {
		//twoOptStarOpenvvj
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;

		table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 2) {
		// outOnevToww_
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;

		table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 3) {
		// outOnevTowwj
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;

		table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 4) {

		// inOnevv_
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;

		table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 5) {

		// inOnevvj
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;

		table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 6) {

		// swapOneOnevw_
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int w__ = customers[w_].prev > input->customerNumer ? 0 : customers[w_].prev;

		int rvId = customers[v].routeId;
		int rwId = customers[w].routeId;

		if (rvId == rwId) {

			if (v == w__) {

				table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

			}
			else {

				table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w__][w_] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

			}

		}
		else {

			table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
			table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
			table[w__][w_] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
			table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

		}

	}
	else if (t.kind == 7) {
		// swapOneOnevwj
		//int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int wjj = customers[wj].next > input->customerNumer ? 0 : customers[wj].next;

		int rvId = customers[v].routeId;
		int rwId = customers[w].routeId;

		if (rvId == rwId) {

			if (v == wj) {
				return false;
			}
			else if (v == wjj) {

				table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

			}
			else {

				table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[wj][wjj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

			}

		}
		else {

			table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
			table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
			table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
			table[wj][wjj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

		}

	}
	else if (t.kind == 8) {

		// swapOneOnevw
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;

		int rvId = customers[v].routeId;
		int rwId = customers[w].routeId;

		if (rvId == rwId) {

			if (v_ == w) {

				table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

			}
			else if (w_ == v) {

				table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

			}
			else {

				table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
				table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

			}

		}
		else {

			table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
			table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
			table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
			table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

		}

	}
	else if (t.kind == 9) {

		//swapThreeTwovvjvjjwwj(v, w); 三换二 v v+ v++ | w w+

		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int vjj = customers[vj].next > input->customerNumer ? 0 : customers[vj].next;
		int v3j = customers[vjj].next > input->customerNumer ? 0 : customers[vjj].next;

		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int wjj = customers[wj].next > input->customerNumer ? 0 : customers[wj].next;

		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[vjj][v3j] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[wj][wjj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);


	}
	else if (t.kind == 10) {

		//swapThreeOnevvjvjjw(v, w); 三换一 v v+ v++ | w

		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int vjj = customers[vj].next > input->customerNumer ? 0 : customers[vj].next;
		int v3j = customers[vjj].next > input->customerNumer ? 0 : customers[vjj].next;

		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;

		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[vjj][v3j] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 11) {
		//swapTwoOnevvjw(v, w); 二换一 v v +  | w
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int vjj = customers[vj].next > input->customerNumer ? 0 : customers[vj].next;

		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;

		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[vj][vjj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 12) {

		//swapOneTwovwwj(v, w); 一换二 v  | w w+

		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;

		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		int wjj = customers[wj].next > input->customerNumer ? 0 : customers[wj].next;

		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[wj][wjj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 13) {

		//outTwovvjTowwj(v, w); 扔两个 v v+  | w w+
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int vjj = customers[vj].next > input->customerNumer ? 0 : customers[vj].next;
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;

		//int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;
		//int wjj = customers[wj].next > input->customerNumer ? 0 : customers[wj].next;

		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[vj][vjj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 14) {
		// v- v | w_ w
		//outTwovv_Toww_  | w-  v- v w

		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int v__ = customers[v_].prev > input->customerNumer ? 0 : customers[v_].prev;
		int vj = customers[v].next > input->customerNumer ? 0 : customers[v].next;
		int w_ = customers[w].prev > input->customerNumer ? 0 : customers[w].prev;

		table[v__][v_] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[v][vj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w_][w] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

	}
	else if (t.kind == 15) {

		//reverse [v,w]
		int v_ = customers[v].prev > input->customerNumer ? 0 : customers[v].prev;
		int wj = customers[w].next > input->customerNumer ? 0 : customers[w].next;


		table[v_][v] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);
		table[w][wj] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

		int fr = solver->getFrontofTwoCus(v, w);
		if (fr == w) {
			std::swap(v, w);
		}

		int pt = v;
		int ptn = customers[pt].next;
		while (pt != w) {
			//debug(pt);
			table[pt][ptn] = iter + aps->yearTabuLen + random->pick(aps->yearTabuRand);

			pt = ptn;
			ptn = customers[ptn].next;
		}

	}
	/*else if (t.kind == 16) {

		for (int i = 0; i < t.ve.size(); i += 2) {
			int v = t.ve[i];
			int w = t.ve[(i + 3) % t.ve.size()];
			sumYear += table[v][w];
		}
		sumYear = sumYear / t.ve.size() * 2;

	}*/
	else {
		Logger::ERROR("sol tabu dont include this move");
	}


	return true;
}

void YearTable::squIterGrowUp(int step) {
	iter += step;
}

}