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
	int turn = 0;
	int numRolls = -1;
	int didTimeTrot = false;
	while (score0 < 100 && score1 < 100){
		if (player == 0){
			numRolls = strat0[score0 * 100 + score1];
			score0 += takeTurn(numRolls, score1);
		}
		else{
			numRolls = strat1[score1 * 100 + score0];
			score1 += takeTurn(numRolls, score0);
		}
		if (isSwap(score0, score1)){
			int temp = score0;
			score0 = score1;
			score1 = temp;
		}
		if (turn % 5 != numRolls || didTimeTrot == true){
			player = other(player);
			didTimeTrot = false;
		}
		else{
			didTimeTrot = true;
		}
		turn += 1;
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

double* spamScoreProbability(int* strat0, int* strat1, int n){
	double* graph = fillArray(new double [200000], 200000, -1.0);
	for (int i = 0; i < n; i++){
		int score0 = 0;
		int score1 = 0;
		int player = 0;
		int turn = 0;
		int numRolls = -1;
		int didTimeTrot = 0;
		while (score0 < 100 && score1 < 100){
			if (graph[getCoordinate(score0, score1, player, didTimeTrot, turn)] == -1){
				graph[getCoordinate(score0, score1, player, didTimeTrot, turn)] = 0.0;
			}
			graph[getCoordinate(score0, score1, player, didTimeTrot, turn)] += 1.0 / (double)(n);

			if (player == 0){
				numRolls = strat0[score0 * 100 + score1];
				score0 += takeTurn(numRolls, score1);
			}
			else{
				numRolls = strat1[score1 * 100 + score0];
				score1 += takeTurn(numRolls, score0);
			}
			if (isSwap(score0, score1)){
				int temp = score0;
				score0 = score1;
				score1 = temp;
			}
			if (turn % 5 != numRolls || didTimeTrot == 1){
				player = other(player);
				didTimeTrot = 0;
			}
			else{
				didTimeTrot = 1;
			}
			turn += 1;
		}
	}
	return graph;
}

double averageWinRate(int* strat0, int* strat1, int n){
	int total = 0;
	for (int i = 0; i < n; i++){
		total += (1 - winner(strat0, strat1));
	}
	return total / (double)(n);
}

double* getExpectedGraph(int* strat0, int* strat1, int score0, int score1, int who, int didTimeTrot, int turnNumber, double* graph){
	int currentCoordinate = getCoordinate(score0, score1, who, didTimeTrot, turnNumber);

	if (graph[currentCoordinate] != -1.0){
		return graph;
	}

	graph[currentCoordinate] = 0.0;

	if (who == 0){
		if (strat0[score0 * 100 + score1] == 0){
			if (isSwap(score0 + freeBacon(score1), score1)){
				if (score0 + freeBacon(score1) < 100){
					if (turnNumber % 5 == 0 && didTimeTrot == 0){
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1, score0 + freeBacon(score1), who, 1, turnNumber + 1, graph)[getCoordinate(score1, score0 + freeBacon(score1), who, 1, turnNumber + 1)];
					}
					else{
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1, score0 + freeBacon(score1), other(who), 0, turnNumber + 1, graph)[getCoordinate(score1, score0 + freeBacon(score1), other(who), 0, turnNumber + 1)];
					}
				}
			}
			else{
				if (score0 + freeBacon(score1) >= 100){
					graph[currentCoordinate] += 1.0;
				}
				else{
					if (turnNumber % 5 == 0 && didTimeTrot == 0){
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0 + freeBacon(score1), score1, who, 1, turnNumber + 1, graph)[getCoordinate(score0 + freeBacon(score1), score1, who, 1, turnNumber + 1)];
					}
					else{
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0 + freeBacon(score1), score1, other(who), 0, turnNumber + 1, graph)[getCoordinate(score0 + freeBacon(score1), score1, other(who), 0, turnNumber + 1)];
					}
				}
			}
		}
		else{
			if (isSwap(score0 + 1, score1)){
				if (score0 + 1 < 100){
					if (turnNumber % 5 == strat0[score0 * 100 + score1] && didTimeTrot == 0){
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1, score0 + 1, who, 1, turnNumber + 1, graph)[getCoordinate(score1, score0 + 1, who, 1, turnNumber + 1)] * (1.0 - pow(5.0 / 6.0, strat0[score0 * 100 + score1]));
					}
					else{
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1, score0 + 1, other(who), 0, turnNumber + 1, graph)[getCoordinate(score1, score0 + 1, other(who), 0, turnNumber + 1)] * (1.0 - pow(5.0 / 6.0, strat0[score0 * 100 + score1]));
					}
				}
			}
			else{
				if (score0 + 1 >= 100){
					graph[currentCoordinate] += (1.0 - pow(5.0 / 6.0, strat0[score0 * 100 + score1]));
				}
				else{
					if (turnNumber % 5 == strat0[score0 * 100 + score1] && didTimeTrot == 0){
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0 + 1, score1, who, 1, turnNumber + 1, graph)[getCoordinate(score0 + 1, score1, who, 1, turnNumber + 1)] * (1.0 - pow(5.0 / 6.0, strat0[score0 * 100 + score1]));
					}
					else{
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0 + 1, score1, other(who), 0, turnNumber + 1, graph)[getCoordinate(score0 + 1, score1, other(who), 0, turnNumber + 1)] * (1.0 - pow(5.0 / 6.0, strat0[score0 * 100 + score1]));
					}
				}
			}
			for (int i = 2 * strat0[score0 * 100 + score1]; i <= 6 * strat0[score0 * 100 + score1]; i++){
				if (isSwap(score0 + i, score1)){
					if (score0 + i < 100){
						if (turnNumber % 5 == strat0[score0 * 100 + score1] && didTimeTrot == 0){
							graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1, score0 + i, who, 1, turnNumber + 1, graph)[getCoordinate(score1, score0 + i, who, 1, turnNumber + 1)] * waysToSumToN(i, strat0[score0 * 100 + score1], 2, 6) * pow(1.0 / 6.0, strat0[score0 * 100 + score1]);
						}
						else{
							graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1, score0 + i, other(who), 0, turnNumber + 1, graph)[getCoordinate(score1, score0 + i, other(who), 0, turnNumber + 1)] * waysToSumToN(i, strat0[score0 * 100 + score1], 2, 6) * pow(1.0 / 6.0, strat0[score0 * 100 + score1]);
						}
					}
				}
				else{
					if (score0 + i >= 100){
						graph[currentCoordinate] += waysToSumToN(i, strat0[score0 * 100 + score1], 2, 6) * pow(1.0 / 6.0, strat0[score0 * 100 + score1]);
					}
					else{
						if (turnNumber % 5 == strat0[score0 * 100 + score1] && didTimeTrot == 0){
							graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0 + i, score1, who, 1, turnNumber + 1, graph)[getCoordinate(score0 + i, score1, who, 1, turnNumber + 1)] * waysToSumToN(i, strat0[score0 * 100 + score1], 2, 6) * pow(1.0 / 6.0, strat0[score0 * 100 + score1]);
						}
						else{
							graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0 + i, score1, other(who), 0, turnNumber + 1, graph)[getCoordinate(score0 + i, score1, other(who), 0, turnNumber + 1)] * waysToSumToN(i, strat0[score0 * 100 + score1], 2, 6) * pow(1.0 / 6.0, strat0[score0 * 100 + score1]);
						}
					}
				}
			}
		}
	}
	else{
		if (strat1[score1 * 100 + score0] == 0){
			if (isSwap(score1 + freeBacon(score0), score0)){
				if (score1 + freeBacon(score0) >= 100){
					graph[currentCoordinate] += 1.0;
				}
				else{
					if (turnNumber % 5 == 0 && didTimeTrot == 0){
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1 + freeBacon(score0), score0, who, 1, turnNumber + 1, graph)[getCoordinate(score1 + freeBacon(score0), score0, who, 1, turnNumber + 1)];
					}
					else{
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1 + freeBacon(score0), score0, other(who), 0, turnNumber + 1, graph)[getCoordinate(score1 + freeBacon(score0), score0, other(who), 0, turnNumber + 1)];
					}
				}
			}
			else{
				if (score1 + freeBacon(score0) < 100){
					if (turnNumber % 5 == 0 && didTimeTrot == 0){
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0, score1 + freeBacon(score0), who, 1, turnNumber + 1, graph)[getCoordinate(score0, score1 + freeBacon(score0), who, 1, turnNumber + 1)];
					}
					else{
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0, score1 + freeBacon(score0), other(who), 0, turnNumber + 1, graph)[getCoordinate(score0, score1 + freeBacon(score0), other(who), 0, turnNumber + 1)];
					}
				}
			}
		}
		else{
			if (isSwap(score1 + 1, score0)){
				if (score1 + 1 >= 100){
					graph[currentCoordinate] += (1.0 - pow(5.0 / 6.0, strat1[score1 * 100 + score0]));
				}
				else{
					if (turnNumber % 5 == strat1[score1 * 100 + score0] && didTimeTrot == 0){
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1 + 1, score0, who, 1, turnNumber + 1, graph)[getCoordinate(score1 + 1, score0, who, 1, turnNumber + 1)] * (1.0 - pow(5.0 / 6.0, strat1[score1 * 100 + score0]));
					}
					else{
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1 + 1, score0, other(who), 0, turnNumber + 1, graph)[getCoordinate(score1 + 1, score0, other(who), 0, turnNumber + 1)] * (1.0 - pow(5.0 / 6.0, strat1[score1 * 100 + score0]));
					}
				}
			}
			else{
				if (score1 + 1 < 100){
					if (turnNumber % 5 == strat1[score1 * 100 + score0] && didTimeTrot == 0){
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0, score1 + 1, who, 1, turnNumber + 1, graph)[getCoordinate(score0, score1 + 1, who, 1, turnNumber + 1)] * (1.0 - pow(5.0 / 6.0, strat1[score1 * 100 + score0]));
					}
					else{
						graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0, score1 + 1, other(who), 0, turnNumber + 1, graph)[getCoordinate(score0, score1 + 1, other(who), 0, turnNumber + 1)] * (1.0 - pow(5.0 / 6.0, strat1[score1 * 100 + score0]));
					}
				}
			}
			for (int i = 2 * strat1[score1 * 100 + score0]; i <= 6 * strat1[score1 * 100 + score0]; i++){
				if (isSwap(score1 + i, score0)){
					if (score1 + i >= 100){
						graph[currentCoordinate] += waysToSumToN(i, strat1[score1 * 100 + score0], 2, 6) * pow(1.0 / 6.0, strat1[score1 * 100 + score0]);
					}
					else{
						if (turnNumber % 5 == strat1[score1 * 100 + score0] && didTimeTrot == 0){
							graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1 + i, score0, who, 1, turnNumber + 1, graph)[getCoordinate(score1 + i, score0, who, 1, turnNumber + 1)] * waysToSumToN(i, strat1[score1 * 100 + score0], 2, 6) * pow(1.0 / 6.0, strat1[score1 * 100 + score0]);
						}
						else{
							graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score1 + i, score0, other(who), 0, turnNumber + 1, graph)[getCoordinate(score1 + i, score0, other(who), 0, turnNumber + 1)] * waysToSumToN(i, strat1[score1 * 100 + score0], 2, 6) * pow(1.0 / 6.0, strat1[score1 * 100 + score0]);
						}
					}
				}
				else{
					if (score1 + i < 100){
						if (turnNumber % 5 == strat1[score1 * 100 + score0] && didTimeTrot == 0){
							graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0, score1 + i, who, 1, turnNumber + 1, graph)[getCoordinate(score0, score1 + i, who, 1, turnNumber + 1)] * waysToSumToN(i, strat1[score1 * 100 + score0], 2, 6) * pow(1.0 / 6.0, strat1[score1 * 100 + score0]);
						}
						else{
							graph[currentCoordinate] += getExpectedGraph(strat0, strat1, score0, score1 + i, other(who), 0, turnNumber + 1, graph)[getCoordinate(score0, score1 + i, other(who), 0, turnNumber + 1)] * waysToSumToN(i, strat1[score1 * 100 + score0], 2, 6) * pow(1.0 / 6.0, strat1[score1 * 100 + score0]);
						}
					}
				}
			}
		}
	}
	return graph;
}

int getCoordinate(int score0, int score1, int who, int didTimeTrot, int turnNumber){
	return 2000 * score0 + 20 * score1 + 10 * who + 5 * didTimeTrot + (turnNumber % 5);
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
			expected[0] = resultVals[1];
			expected[1] = resultVals[0];
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


double* fullExpectedTable(int score0, int score1, int who, int didTimeTrot, int turnNumber, double* graph){
	for (int i = 0; i < 11; i++){
		int currentCoordinate = getCoordinate2(score0, score1, who, didTimeTrot, turnNumber, i, 0);
		if (graph[currentCoordinate] != -1.0){
			return graph;
		}
		graph[currentCoordinate] = 0.0;
		graph[currentCoordinate + 1] = 0.0;

		if (who == 0){
			if (i == 0){
				if (isSwap(score0 + freeBacon(score1), score1)){
					if (score0 + freeBacon(score1) < 100){
						if (turnNumber % 5 == 0 && didTimeTrot == 0){
							int maxIdx = getMaximum(score1, score0 + freeBacon(score1), who, 1, turnNumber + 1, fullExpectedTable(score1, score0 + freeBacon(score1), who, 1, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx];
							graph[currentCoordinate + 1] += graph[maxIdx + 1];
						}
						else{
							int maxIdx = getMaximum(score1, score0 + freeBacon(score1), other(who), 0, turnNumber + 1, fullExpectedTable(score1, score0 + freeBacon(score1), other(who), 0, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx];
							graph[currentCoordinate + 1] += graph[maxIdx + 1];
						}
					}
					else{
						graph[currentCoordinate] += score1;
						graph[currentCoordinate + 1] += score0 + freeBacon(score1);
					}
				}
				else{
					if (score0 + freeBacon(score1) >= 100){
						graph[currentCoordinate] += score0 + freeBacon(score1);
						graph[currentCoordinate + 1] += score1;
					}
					else{
						if (turnNumber % 5 == 0 && didTimeTrot == 0){
							int maxIdx = getMaximum(score0 + freeBacon(score1), score1, who, 1, turnNumber + 1, fullExpectedTable(score0 + freeBacon(score1), score1, who, 1, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx];
							graph[currentCoordinate + 1] += graph[maxIdx + 1];
						}
						else{
							int maxIdx = getMaximum(score0 + freeBacon(score1), score1, other(who), 0, turnNumber + 1, fullExpectedTable(score0 + freeBacon(score1), score1, other(who), 0, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx];
							graph[currentCoordinate + 1] += graph[maxIdx + 1];
						}
					}
				}
			}
			else{
				double prob = (1.0 - pow(5.0 / 6.0, i));
				if (isSwap(score0 + 1, score1)){
					if (score0 + 1 < 100){
						if (turnNumber % 5 == i && didTimeTrot == 0){
							int maxIdx = getMaximum(score1, score0 + 1, who, 1, turnNumber + 1, fullExpectedTable(score1, score0 + 1, who, 1, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx] * prob;
							graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
						}
						else{
							int maxIdx = getMaximum(score1, score0 + 1, other(who), 0, turnNumber + 1, fullExpectedTable(score1, score0 + 1, other(who), 0, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx] * prob;
							graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
						}
					}
					else{
						graph[currentCoordinate] += score1 * prob;
						graph[currentCoordinate + 1] += (score0 + 1) * prob;
					}
				}
				else{
					if (score0 + 1 >= 100){
						graph[currentCoordinate] += (score0 + 1) * prob;
						graph[currentCoordinate + 1] += score1 * prob;
					}
					else{
						if (turnNumber % 5 == i && didTimeTrot == 0){
							int maxIdx = getMaximum(score0 + 1, score1, who, 1, turnNumber + 1, fullExpectedTable(score0 + 1, score1, who, 1, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx] * prob;
							graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
						}
						else{
							int maxIdx = getMaximum(score0 + 1, score1, other(who), 0, turnNumber + 1, fullExpectedTable(score0 + 1, score1, other(who), 0, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx] * prob;
							graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
						}
					}
				}
				for (int x = 2 * i; x <= 6 * i; x++){
					prob = waysToSumToN(x, i, 2, 6) * pow(1.0 / 6.0, i);
					if (isSwap(score0 + x, score1)){
						if (score0 + x < 100){
							if (turnNumber % 5 == i && didTimeTrot == 0){
								int maxIdx = getMaximum(score1, score0 + x, who, 1, turnNumber + 1, fullExpectedTable(score1, score0 + x, who, 1, turnNumber + 1, graph));
								graph[currentCoordinate] += graph[maxIdx] * prob;
								graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
							}
							else{
								int maxIdx = getMaximum(score1, score0 + x, other(who), 0, turnNumber + 1, fullExpectedTable(score1, score0 + x, other(who), 0, turnNumber + 1, graph));
								graph[currentCoordinate] += graph[maxIdx] * prob;
								graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
							}
						}
						else{
							graph[currentCoordinate] += score1 * prob;
							graph[currentCoordinate + 1] += (score0 + x) * prob;
						}
					}
					else{
						if (score0 + x >= 100){
							graph[currentCoordinate] += (score0 + x) * prob;
							graph[currentCoordinate + 1] += score1 * prob;
						}
						else{
							if (turnNumber % 5 == i && didTimeTrot == 0){
								int maxIdx = getMaximum(score0 + x, score1, who, 1, turnNumber + 1, fullExpectedTable(score0 + x, score1, who, 1, turnNumber + 1, graph));
								graph[currentCoordinate] += graph[maxIdx] * prob;
								graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
							}
							else{
								int maxIdx = getMaximum(score0 + x, score1, other(who), 0, turnNumber + 1, fullExpectedTable(score0 + x, score1, other(who), 0, turnNumber + 1, graph));
								graph[currentCoordinate] += graph[maxIdx] * prob;
								graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
							}
						}
					}
				}
			}
		}
		else{
			if (i == 0){
				if (isSwap(score1 + freeBacon(score0), score0)){
					if (score1 + freeBacon(score0) >= 100){
						graph[currentCoordinate] += score1 + freeBacon(score0);
						graph[currentCoordinate + 1] += score0;
					}
					else{
						if (turnNumber % 5 == 0 && didTimeTrot == 0){
							int maxIdx = getMaximum(score1 + freeBacon(score0), score0, who, 1, turnNumber + 1, fullExpectedTable(score1 + freeBacon(score0), score0, who, 1, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx];
							graph[currentCoordinate + 1] += graph[maxIdx + 1];
						}
						else{
							int maxIdx = getMaximum(score1 + freeBacon(score0), score0, other(who), 0, turnNumber + 1, fullExpectedTable(score1 + freeBacon(score0), score0, other(who), 0, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx];
							graph[currentCoordinate + 1] += graph[maxIdx + 1];
						}
					}
				}
				else{
					if (score1 + freeBacon(score0) < 100){
						if (turnNumber % 5 == 0 && didTimeTrot == 0){
							int maxIdx = getMaximum(score0, score1 + freeBacon(score0), who, 1, turnNumber + 1, fullExpectedTable(score0, score1 + freeBacon(score0), who, 1, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx];
							graph[currentCoordinate + 1] += graph[maxIdx + 1];
						}
						else{
							int maxIdx = getMaximum(score0, score1 + freeBacon(score0), other(who), 0, turnNumber + 1, fullExpectedTable(score0, score1 + freeBacon(score0), other(who), 0, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx];
							graph[currentCoordinate + 1] += graph[maxIdx + 1];
						}
					}
					else{
						graph[currentCoordinate] += score0;
						graph[currentCoordinate + 1] += score1 + freeBacon(score0);
					}
				}
			}
			else{
				double prob = (1.0 - pow(5.0 / 6.0, i));
				if (isSwap(score1 + 1, score0)){
					if (score1 + 1 >= 100){
						graph[currentCoordinate] += (score1 + 1) * prob;
						graph[currentCoordinate + 1] += score0 * prob;
					}
					else{
						if (turnNumber % 5 == i && didTimeTrot == 0){
							int maxIdx = getMaximum(score1 + 1, score0, who, 1, turnNumber + 1, fullExpectedTable(score1 + 1, score0, who, 1, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx] * prob;
							graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
						}
						else{
							int maxIdx = getMaximum(score1 + 1, score0, other(who), 0, turnNumber + 1, fullExpectedTable(score1 + 1, score0, other(who), 0, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx] * prob;
							graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
						}
					}
				}
				else{
					if (score1 + 1 < 100){
						if (turnNumber % 5 == i && didTimeTrot == 0){
							int maxIdx = getMaximum(score0, score1 + 1, who, 1, turnNumber + 1, fullExpectedTable(score0, score1 + 1, who, 1, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx] * prob;
							graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
						}
						else{
							int maxIdx = getMaximum(score0, score1 + 1, other(who), 0, turnNumber + 1, fullExpectedTable(score0, score1 + 1, other(who), 0, turnNumber + 1, graph));
							graph[currentCoordinate] += graph[maxIdx] * prob;
							graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
						}
					}
					else{
						graph[currentCoordinate] += score0 * prob;
						graph[currentCoordinate + 1] += (score1 + 1) * prob;
					}
				}
				for (int x = 2 * i; x <= 6 * i; x++){
					prob = waysToSumToN(x, i, 2, 6) * pow(1.0 / 6.0, i);
					if (isSwap(score1 + x, score0)){
						if (score1 + x >= 100){
							graph[currentCoordinate] += (score1 + x) * prob;
							graph[currentCoordinate + 1] += score0 * prob;
						}
						else{
							if (turnNumber % 5 == i && didTimeTrot == 0){
								int maxIdx = getMaximum(score1 + x, score0, who, 1, turnNumber + 1, fullExpectedTable(score1 + x, score0, who, 1, turnNumber + 1, graph));
								graph[currentCoordinate] += graph[maxIdx] * prob;
								graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
							}
							else{
								int maxIdx = getMaximum(score1 + x, score0, other(who), 0, turnNumber + 1, fullExpectedTable(score1 + x, score0, other(who), 0, turnNumber + 1, graph));
								graph[currentCoordinate] += graph[maxIdx] * prob;
								graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
							}
						}
					}
					else{
						if (score1 + x < 100){
							if (turnNumber % 5 == i && didTimeTrot == 0){
								int maxIdx = getMaximum(score0, score1 + x, who, 1, turnNumber + 1, fullExpectedTable(score0, score1 + x, who, 1, turnNumber + 1, graph));
								graph[currentCoordinate] += graph[maxIdx] * prob;
								graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
							}
							else{
								int maxIdx = getMaximum(score0, score1 + x, other(who), 0, turnNumber + 1, fullExpectedTable(score0, score1 + x, other(who), 0, turnNumber + 1, graph));
								graph[currentCoordinate] += graph[maxIdx] * prob;
								graph[currentCoordinate + 1] += graph[maxIdx + 1] * prob;
							}
						}
						else{
							graph[currentCoordinate] += score0 * prob;
							graph[currentCoordinate + 1] += (score1 + x) * prob;
						}
					}
				}
			}
		}
	}
	return graph;
}

int getCoordinate2(int score0, int score1, int who, int didTimeTrot, int turnNumber, int numRolls, int whichScore){
	return 44000 * score0 + 440 * score1 + 220 * who + 110 * didTimeTrot + 22 * (turnNumber % 5) + 2 * numRolls + whichScore;
}

int getMaximum(int score0, int score1, int who, int didTimeTrot, int turnNumber, double* graph){
	int maxVal = -10000000;
	int maxCoordinate = 0;
	for (int i = 0; i < 11; i++){
		if (graph[getCoordinate2(score0, score1, who,  didTimeTrot, turnNumber, i, who)] - graph[getCoordinate2(score0, score1, who,  didTimeTrot, turnNumber, i, other(who))] > maxVal){
			maxVal = graph[getCoordinate2(score0, score1, who,  didTimeTrot, turnNumber, i, who)] - graph[getCoordinate2(score0, score1, who,  didTimeTrot, turnNumber, i, other(who))];
			maxCoordinate = getCoordinate2(score0, score1, who,  didTimeTrot, turnNumber, i, 0);
		}
	}
	return maxCoordinate;
}








int* generateFinalStratTable(int n, double* probabilityOfReaching){
	int* strat = new int [10000];
	for (int i = 0; i < 10000; i++){
		strat[i] = getMaxRoll(i / 100, i % 100, n)[2];
	}
	return strat;
}

int* generateFullTable(double* weights){
	double* table = fullExpectedTable();

	int* newTable = new int [10000];
	for (int i = 0; i < 100; i++){
		for (int x = 0; x < 100; x++){
			double maxVal = -10000000.0;
			int maxRoll = 0;
			double totalWeight = 0.0;
			for (int who = 0; who < 2; who++){
				for (int didTimeTrot = 0; didTimeTrot < 2; didTimeTrot++){
					for (int turnNumber = 0; turnNumber < 5; turnNumber++){
						totalWeight += weights[getCoordinate(i, x, who, didTimeTrot, turnNumber)];
					}
				}
			}


			for (int rolls = 0; rolls < 11; rolls++){
				double weightedExpected = 0.0;
				double weightedExpectedOpponent = 0.0;
				for (int who = 0; who < 2; who++){
					for (int didTimeTrot = 0; didTimeTrot < 2; didTimeTrot++){
						for (int turnNumber = 0; turnNumber < 5; turnNumber++){
							weightedExpected += table[getCoordinate2(i, x, who, didTimeTrot, turnNumber, rolls, who)] * weights[getCoordinate(i, x, who, didTimeTrot, turnNumber)] / totalWeight;
							weightedExpectedOpponent += table[getCoordinate2(i, x, who, didTimeTrot, turnNumber, rolls, other(who))] * weights[getCoordinate(i, x, who, didTimeTrot, turnNumber)] / totalWeight;
						}
					}
				}

				if (weightedExpected - weightedExpectedOpponent > maxVal){
					maxVal = weightedExpected - weightedExpectedOpponent;
					maxRoll = rolls;
				}
			}
			newTable[i * 100 + x] = maxRoll;
		}
	}
	return newTable;
}








