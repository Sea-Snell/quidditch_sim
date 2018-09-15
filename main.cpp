#include "HogLogic.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main(){
	srand((unsigned int) time(NULL));

	int* strat0 = generateFinalStratTable();
	int* strat1 = alwaysRollStrat(4);
	double* winRate = getExpectedGraph(strat0, strat1);
	double winRate2 = averageWinRate(strat0, strat1, 10000000);
	cout << winRate[0] << endl;
	cout << winRate2 << endl;
	return 0;
}