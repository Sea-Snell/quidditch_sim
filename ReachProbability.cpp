#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
#include "DataManager.cpp"

using namespace std;

class ProbabilityMapper
{
public:
	int* strat0;
	int* strat1;
	vector<vector<float>> turnProbs;
	ProbabilityMapper(){
		this->turnProbs = turnMap();


	}

	vector<vector<float> > turnMap(){
		vector<float> emptyProbs;
		for(float i = 0; i<=60; i++){
			emptyProbs.push_back(i);
			emptyProbs.push_back(0.0);
		}

		vector<vector<float> > possTurns;
		vector<float> lay0;
		lay0.push_back(0.0);
		lay0.push_back(1.0);
		lay0.push_back(1.0);
		lay0.push_back(0.0);
		possTurns.push_back(lay0);
		for(int i = 0; i<10;i++){
			vector<float> tempLayer = emptyProbs;
			for(int j = 0; j< possTurns[i].size(); j+=2){
				if(possTurns[i][j] == 1.0){
					tempLayer[3] += possTurns[i][j+1] + ((1-possTurns[i][j+1])*1/6.0);
				}else{
					for(int k = 2; k <= 6; k++){
						tempLayer[2*(possTurns[i][j] + k) + 1] += possTurns[i][j+1] * 1/6.0;
					}
				}
			}
			for(int j = 121; j >= 0; j-=2){
				if(tempLayer[j] == 0.0){
					tempLayer.erase(tempLayer.begin() + j-1, tempLayer.begin() + j+1);
				}
			}
			possTurns.push_back(tempLayer);
		}
		possTurns[0].erase(possTurns[0].begin()+2, possTurns[0].begin() +4);
		return possTurns;
	}

	vector<vector<vector<float> > > ProbabilityMap(int* strat0, int* strat1, int score0 = 0, int score1 = 0, int turn = 0, int who = 0, int initProb = 1.0, int trotLast = 0){
		this->strat0 = strat0;
		this->strat1 = strat1;
		queue.clear();
		for(int i = 0; i < 199; i++){
			vector<vector<int>> temp;
			this->queue.push_back(temp);
		}
		reached.clear();
		for(int i = 0; i < 100; i++){
			vector<vector<bool>> temp;
			this->reached.push_back(temp);
			for(int j = 0; j < 100; j++){
				vector<bool> temp2	(20,true);
				this->reached[i].push_back(temp2);
			}
		}

		output.clear();
		for(int i = 0; i < 100; i++){
			vector<vector<float>> temp;
			this->output.push_back(temp);
			for(int j = 0; j < 100; j++){
				vector<float> temp2	(20,0.0);
				this->output[i].push_back(temp2);
			}
		}

		output[score0][score1][turn%5 + 5* who + 10* trotLast] = initProb;

		vector<int> startPoint = {score0, score1, turn%5 + 5* who, trotLast};
		queue[slice(score0,score1)].push_back(startPoint);
		for(int i = slice(score0, score1); i < 199; i++){
			for(int j = 0; j < queue[i].size(); j++){
				queueTurn(queue[i][j][0], queue[i][j][1], queue[i][j][2], queue[i][j][3], output[queue[i][j][0]][queue[i][j][1]][queue[i][j][2] + 10* queue[i][j][3]]);
			}
			cout << i << endl;
		}
		return output;
	}


private:
	vector<vector<vector<int> > > queue;
	vector<vector<vector<float> > > output;
	vector<vector<vector<bool> > > reached;

	int gridIndex(int score0, int score1){
			return score0*100 + score1;
	}

	int slice(int score0, int score1){
		 return score0 + score1;
	}

	int getDigitDifference(int score){
		return abs((score % 100) / 10 - score % 10);
	}

	int freeBacon(int score1){
		return max(1, 2 * (score1 / 10) - (score1 % 10));
	}

	bool isSwap(int score0, int score1){
		return getDigitDifference(score0) == getDigitDifference(score1);
	}

	void queueTurn(int score0, int score1, int turnState, int trotLast, float probability){
		int* strat;
		int aScore;
		int bScore;
		if(turnState < 5){
			aScore = score0;
			bScore = score1;
			strat = strat0;
		}else{
			aScore = score1;
			bScore = score0;
			strat = strat1;
		}
		for(int i = 0; i< queue[slice(score0, score1)].size(); i++){
			for(int j = 0; j < turnProbs[strat[gridIndex(aScore, bScore)]].size(); j+=2){
				int tempAS = aScore;
				int tempBS = bScore;
				int rollCo = turnProbs[strat[gridIndex(aScore, bScore)]][j];
				if(rollCo == 0.0){
					tempAS += freeBacon(tempBS);
				}else{
					tempAS += rollCo;
				}

				if(isSwap(tempAS, tempBS)){
					int swapper = tempBS;
					tempBS = tempAS;
					tempAS = swapper;
				}

				if(turnState >= 5){
					int swapper = tempBS;
					tempBS = tempAS;
					tempAS = swapper;
				}

				if(tempBS > 99 || tempAS > 99){
					continue;
				}
				//turn calculation
				int nextTurn;
				int trot = 0;

				if(trotLast == 1){
					trot = 0;
					nextTurn = (turnState + 1)%5 + (turnState < 5 ? 5 : 0);
				}else if(turnState%5 == rollCo){
					trot = 1;
					nextTurn = (turnState + 1)%5 + (turnState < 5 ? 0 : 5);
				}else{
					trot = 0;
					nextTurn = (turnState + 1)%5 + (turnState < 5 ? 5 : 0);
				}
				vector<int> queueEntry = {tempAS, tempBS, nextTurn, trot};
				if(reached[tempAS][tempBS][nextTurn + trot * 10]){
					queue[slice(tempAS,tempBS)].push_back(queueEntry);
					reached[tempAS][tempBS][nextTurn + trot * 10] = false;
				}
				output[tempAS][tempBS][nextTurn + trot * 10] += probability * turnProbs[strat[gridIndex(aScore, bScore)]][j+1];
			}
		}
	}
};



int main(){
	ProbabilityMapper testmap;
	int str0 [10000];
	int str1 [10000];
	for(int i = 0; i<= 10000; i++){
		str0[i] = 1;
		str1[i] = 1;
	}

	vector<vector<vector<float>>> outData = testmap.ProbabilityMap(str0,str1);
	for(int i = 0; i < 10; i++){
		for(int j = 0; j< 10 ; j++){
			for(int k = 0; k < 20; k++){
				cout << outData[i][j][k] << endl;
			}
		}
	}

}
