#include "HogLogic.hpp"
#include <iostream>
#include <stdlib.h>
#include <time.h>

using namespace std;

int main(){
	srand((unsigned int) time(NULL));

	int* strat0 = generateFinalStratTable(6);
	int* strat1 = swapStrat();
	double winRate = averageWinRate(strat0, strat1);
	cout << winRate << endl;
	return 0;
}