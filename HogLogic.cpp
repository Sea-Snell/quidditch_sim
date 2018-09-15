#include "HogLogic.hpp"
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <math.h>
#include <iostream>

using namespace std;

int dice(){
	return rand() % 6 + 1;
}


int rollDice(int numRolls){
	int total = 0;
    while (numRolls > 0){
        int rollVal = dice();
        if (rollVal == 1){
            total = -1;
        }
        else if (total != -1){
            total += rollVal;
        }
        numRolls -= 1;
    }
    return max(total, 1);
}

int freeBacon(int score1){
	return max(1, 2 * (score1 / 10) - (score1 % 10));
}

int takeTurn(int numRolls, int score1){
	if (numRolls == 0){
		return freeBacon(score1);
	}
	return rollDice(numRolls);
}

int getDigitDifference(int score){
	return abs((score % 100) / 10 - score % 10);
}

bool isSwap(int score0, int score1){
	return getDigitDifference(score0) == getDigitDifference(score1);
}

int other(int player){
	return 1 - player;
}

int* play(int* strat0, int* strat1, int score0, int score1){
	int player = 0;
	while (score0 < 100 && score1 < 100){
		if (player == 0){
			score0 += takeTurn(strat0[score0 * 100 + score1], score1);
		}
		else{
			score1 += takeTurn(strat1[score1 * 100 + score0], score0);
		}
		if (isSwap(score0, score1)){
			int temp = score0;
			score0 = score1;
			score1 = temp;
		}
		player = other(player);
	}
	return new int [2] {score0, score1};
}

int winner(int* strat0, int* strat1){
	int* result = play(strat0, strat1);
	if (result[0] > result[1]){
		return 0;
	}
	return 1;
}




double* fillArray(double* items, int size, double n){
	for(int i = 0; i < size; i++){
		items[i] = n;
	}
	return items;
}

double averageWinRate(int* strat0, int* strat1, int n){
	int total = 0;
	for (int i = 0; i < n; i++){
		total += (1 - winner(strat0, strat1));
		total += winner(strat1, strat0);
	}
	return total / (n * 2.0);
}

double* getExpectedGraph(int* strat0, int* strat1, int score0, int score1, int who, double* graph){
	if (graph[score0 * 100 + score1] != -1.0){
		return graph;
	}

	graph[score0 * 100 + score1] = 0.0;

	if (who == 0){
		if (strat0[score0 * 100 + score1] == 0){
			if (isSwap(score0 + freeBacon(score1), score1)){
				if (score0 + freeBacon(score1) < 100){
					graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score1, score0 + freeBacon(score1), other(who), graph)[score1 * 100 + score0 + freeBacon(score1)];
				}
			}
			else{
				if (score0 + freeBacon(score1) >= 100){
					graph[score0 * 100 + score1] += 1.0;
				}
				else{
					graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score0 + freeBacon(score1), score1, other(who), graph)[(score0 + freeBacon(score1)) * 100 + score1];
				}
			}
		}
		else{
			if (isSwap(score0 + 1, score1)){
				if (score0 + 1 < 100){
					graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score1, score0 + 1, other(who), graph)[score1 * 100 + score0 + 1] * (1.0 - pow(5.0 / 6.0, strat0[score0 * 100 + score1]));
				}
			}
			else{
				if (score0 + 1 >= 100){
					graph[score0 * 100 + score1] += (1.0 - pow(5.0 / 6.0, strat0[score0 * 100 + score1]));
				}
				else{
					graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score0 + 1, score1, other(who), graph)[(score0 + 1) * 100 + score1] * (1.0 - pow(5.0 / 6.0, strat0[score0 * 100 + score1]));
				}
			}
			for (int i = 2 * strat0[score0 * 100 + score1]; i <= 6 * strat0[score0 * 100 + score1]; i++){
				if (isSwap(score0 + i, score1)){
					if (score0 + i < 100){
						graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score1, score0 + i, other(who), graph)[score1 * 100 + score0 + i] * waysToSumToN(i, strat0[score0 * 100 + score1], 2, 6) * pow(1.0 / 6.0, strat0[score0 * 100 + score1]);
					}
				}
				else{
					if (score0 + i >= 100){
						graph[score0 * 100 + score1] += waysToSumToN(i, strat0[score0 * 100 + score1], 2, 6) * pow(1.0 / 6.0, strat0[score0 * 100 + score1]);
					}
					else{
						graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score0 + i, score1, other(who), graph)[(score0 + i) * 100 + score1] * waysToSumToN(i, strat0[score0 * 100 + score1], 2, 6) * pow(1.0 / 6.0, strat0[score0 * 100 + score1]);
					}
				}
			}
		}
	}
	else{
		if (strat1[score1 * 100 + score0] == 0){
			if (isSwap(score1 + freeBacon(score0), score0)){
				if (score1 + freeBacon(score0) >= 100){
					graph[score0 * 100 + score1] += 1.0;
				}
				else{
					graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score1 + freeBacon(score0), score0, other(who), graph)[(score1 + freeBacon(score0)) * 100 + score0];
				}
			}
			else{
				if (score1 + freeBacon(score0) < 100){
					graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score0, score1 + freeBacon(score0), other(who), graph)[score0 * 100 + score1 + freeBacon(score0)];
				}
			}
		}
		else{
			if (isSwap(score1 + 1, score0)){
				if (score1 + 1 >= 100){
					graph[score0 * 100 + score1] += (1.0 - pow(5.0 / 6.0, strat1[score1 * 100 + score0]));
				}
				else{
					graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score1 + 1, score0, other(who), graph)[(score1 + 1) * 100 + score0] * (1.0 - pow(5.0 / 6.0, strat1[score1 * 100 + score0]));
				}
			}
			else{
				if (score1 + 1 < 100){
					graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score0, score1 + 1, other(who), graph)[score0 * 100 + score1 + 1] * (1.0 - pow(5.0 / 6.0, strat1[score1 * 100 + score0]));
				}
			}
			for (int i = 2 * strat1[score1 * 100 + score0]; i <= 6 * strat1[score1 * 100 + score0]; i++){
				if (isSwap(score1 + i, score0)){
					if (score1 + i >= 100){
						graph[score0 * 100 + score1] += waysToSumToN(i, strat1[score1 * 100 + score0], 2, 6) * pow(1.0 / 6.0, strat1[score1 * 100 + score0]);
					}
					else{
						graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score1 + i, score0, other(who), graph)[(score1 + i) * 100 + score0] * waysToSumToN(i, strat1[score1 * 100 + score0], 2, 6) * pow(1.0 / 6.0, strat1[score1 * 100 + score0]);
					}
				}
				else{
					if (score1 + i < 100){
						graph[score0 * 100 + score1] += getExpectedGraph(strat0, strat1, score0, score1 + i, other(who), graph)[score0 * 100 + score1 + i] * waysToSumToN(i, strat1[score1 * 100 + score0], 2, 6) * pow(1.0 / 6.0, strat1[score1 * 100 + score0]);
					}
				}
			}
		}
	}
	return graph;
}




int* alwaysRollStrat(int n){
	int* strat = new int [10000];
	for (int i = 0; i < 10000; i++){
		strat[i] = n;
	}
	return strat;
}

int* baconStrat(int margin, int numRolls){
	int* strat = new int [10000];
	for (int i = 0; i < 10000; i++){
		if (freeBacon(i % 100) >= margin){
			strat[i] = 0;
		}
		else{
			strat[i] = numRolls;
		}
	}
	return strat;
}

int* swapStrat(int margin, int numRolls){
	int* strat = new int [10000];
	for (int i = 0; i < 10000; i++){
		int baconScore = freeBacon(i % 100);
		if (isSwap(baconScore + (i / 100), i % 100)){
			if (baconScore + (i / 100) < (i % 100)){
				strat[i] = 0;
			}
			else{
				strat[i] = numRolls;
			}
		}
		else if(baconScore >= margin){
			strat[i] = 0;
		}
		else{
			strat[i] = numRolls;
		}
	}
	return strat;
}






vector<int> getSwapRolls(int score0, int score1){
	vector<int> swapRolls;
	swapRolls.reserve(20);

	int goalDifference = getDigitDifference(score1);
	int tensDigit = (score0 / 10) % 10;

	for(int i = tensDigit; i < tensDigit + 7; i++){
		if (i - goalDifference >= 0){
			int newScore = (i * 11 - goalDifference) - score0;
			if (newScore > 0){
				swapRolls.push_back(newScore);
			}
		}
		if (i + goalDifference <= 9){
			int newScore = (i * 11 + goalDifference) - score0;
			if (newScore > 0){
				swapRolls.push_back(newScore);
			}
		}
	}
	return swapRolls;
}

double calculateExpectedRoll(int numRolls){
	int totalExpected = 0;
	for (int i = 2 * numRolls; i <= 6 * numRolls; i++){
		totalExpected += waysToSumToN(i, numRolls, 2, 6) * i;
	}
	return (totalExpected / pow(6, numRolls)) + (1.0 - pow(5.0 / 6.0, numRolls));
}

double waysToSumToN(int n, int numRolls, int lowerBound, int upperBound){
	n -= (lowerBound - 1) * numRolls;
	double total = 0;
	for (int i = 0; i < ((n / (upperBound - lowerBound + 2)) + 1); i++){
		total += aChooseB(numRolls, i) * aChooseB(n - (upperBound - lowerBound + 1) * i - 1, numRolls - 1) * pow(-1, i);
	}
	return total;
}

double aChooseB(int a, int b){
	if (a < b){
		return 0.0;
	}
	double total = 1;
	int numTerms = min(b, a - b);
	for (int i = 0; i < numTerms; i++){
		total *= ((double)(a - i)) / ((double)(numTerms - i));
	}
	return total;
}

double* generateProbabilitiesTable(){
	double* probabilitiesTable = new double [10];
	for (int i = 1; i < 11; i++){
		probabilitiesTable[i - 1] = calculateExpectedRoll(i);
	}
	return probabilitiesTable;
}

double* calculateExpected(int score0, int score1, int numRolls){
	double probabilitiesTable [10] = {3.5, 5.86111, 7.36574, 8.23302, 8.63542, 8.70257, 8.53518, 8.2096, 7.78323, 7.29872};
	vector<int> swapRolls = getSwapRolls(score0, score1);
	double opponentExpected = 0.0;
	double expectedValue = 0.0;

	if (numRolls == 0){
		expectedValue = freeBacon(score1);
		for(int i = 0; i < swapRolls.size(); i++){
			if (swapRolls[i] == expectedValue){
				expectedValue = (score1 - (score0 + expectedValue));
				opponentExpected = -expectedValue;
				break;
			}
		}
	}
	else{
		expectedValue = probabilitiesTable[numRolls];
		double numCombos = pow(6, numRolls);
		double waysToSwap1 = numCombos - pow(5, numRolls);

		for(int i = 0; i < swapRolls.size(); i++){
			double scoreDelta = (score1 - (score0 + i));
			if (swapRolls[i] >= 2 * numRolls && swapRolls[i] <= 6 * numRolls){
				int waysToSwap = waysToSumToN(swapRolls[i], numRolls, 2, 6);
				expectedValue = (expectedValue * numCombos - waysToSwap * swapRolls[i] + waysToSwap * scoreDelta) / numCombos;
				opponentExpected = (opponentExpected * numCombos + waysToSwap * (-scoreDelta)) / numCombos;
			}
			else if (swapRolls[i] == 1){
				expectedValue = (expectedValue * numCombos - waysToSwap1 + waysToSwap1 * scoreDelta) / numCombos;
				opponentExpected = (opponentExpected * numCombos + waysToSwap1 * (-scoreDelta)) / numCombos;
			}
		}
	}
	return new double [2] {expectedValue + score0, opponentExpected + score1};
}

double* getMaxRoll(int score0, int score1, int turnsAhead){
	double bestRoll = 0;
	double maxDiff = -1000000;
	double maxPlayerScore = 0;
	double maxOpponentScore = 0;

	for (int i = 0; i < 11; i++){
		double* expected = calculateExpected((int)(score0 + 0.5), (int)(score1 + 0.5), i);
		if (turnsAhead > 1 && ((int)(score0 + 0.5) < 100) && ((int)(score1 + 0.5) < 100)){
			double* resultVals = getMaxRoll(expected[1], expected[0], turnsAhead - 1);
		}
		if (expected[0] - expected[1] > maxDiff){
			maxDiff = expected[0] - expected[1];
			maxPlayerScore = expected[0];
			maxOpponentScore = expected[1];
			bestRoll = (double)(i);
		}
	}
	return new double [3] {maxPlayerScore, maxOpponentScore, bestRoll};
}

int* generateFinalStratTable(int n){
	int* strat = new int [10000];
	for (int i = 0; i < 10000; i++){
		strat[i] = getMaxRoll(i / 100, i % 100, n)[2];
	}
	return strat;
}








