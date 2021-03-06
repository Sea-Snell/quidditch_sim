#ifndef HOGLOGIC_H
#define HOGLOGIC_H

#include <vector>
using namespace std;

int dice();
int rollDice(int numRolls);
int freeBacon(int score1);
int getDigitDifference(int score);
bool isSwap(int score0, int score1);
int other(int player);
int* play(int* strat0, int* strat1, int score0 = 0, int score1 = 0);
int winner(int* strat0, int* strat1);

double* fillArray(double* items, int size, double n);
double* spamScoreProbability(int* strat0, int* strat1, int n);
double averageWinRate(int* strat0, int* strat1, int n = 10000);
double* spamScoreProbability(int* strat0, int* strat1, int n = 10000);
double* getExpectedGraph(int* strat0, int* strat1, int score0 = 0, int score1 = 0, int who = 0, int didTimeTrot = 0, int turnNumber = 0, double* graph = fillArray(new double [200000], 200000, -1.0));
int getCoordinate(int score0, int score1, int who, int didTimeTrot, int turnNumber);

int* alwaysRollStrat(int n);
int* baconStrat(int margin = 8, int numRolls = 4);
int* swapStrat(int margin = 8, int numRolls = 4);

vector<int> getSwapRolls(int score0, int score1);
double calculateExpectedRoll(int numRolls);
double waysToSumToN(int n, int numRolls, int lowerBound = 1, int upperBound = 6);
double aChooseB(int a, int b);
double* generateProbabilitiesTable();
double* calculateExpected(int score0, int score1, int numRolls);
double* getMaxRoll(int score0, int score1, int turnsAhead = 1);
double* fullExpectedTable(int score0 = 0, int score1 = 0, int who = 0, int didTimeTrot = 0, int turnNumber = 0, double* graph = fillArray(new double [4400000], 4400000, -1.0));
int getCoordinate2(int score0, int score1, int who, int didTimeTrot, int turnNumber, int numRolls, int whichScore);
int getMaximum(int score0, int score1, int who, int didTimeTrot, int turnNumber, double* graph);


int* generateFinalStratTable(int n = 1, double* probabilityOfReaching = fillArray(new double [200000], 200000, -1.0));
int* generateFullTable(double* weights);


#endif