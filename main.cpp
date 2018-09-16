#include "HogLogic.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main(){
	srand((unsigned int) time(NULL));

	int* strat0 = generateFinalStratTable(1);
	int* strat1 = swapStrat();
	double* winRateA = getExpectedGraph(strat0, strat1);
	double* winRateB = getExpectedGraph(strat1, strat0);
	double winRateC = averageWinRate(strat0, strat1, 10000);
	double winRateD = averageWinRate(strat1, strat0, 10000);
	cout << winRateA[0] * 0.5 + (1.0 - winRateB[0]) * 0.5 << endl;
	cout << winRateC * 0.5 + (1.0 - winRateD) * 0.5 << endl;
	return 0;
}