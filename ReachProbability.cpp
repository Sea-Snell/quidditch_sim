#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "DataManager.cpp"

using namespace std;

class ProbabilityMapper
{
public:
	ProbabilityMapper(){
		for(int i = 0; i < 199; i++){
			vector<vector<int>> temp;
			this->queue.push_back(temp);
		}
		for(int i = 0; i < 100; i++){
			vector<vector<float>> temp;
			this->output.push_back(temp);
			for(int j = 0; j < 100; j++){
				vector<float> temp2	(20,0.0);
				this->output[i].push_back(temp2);
			}
		}

	}
	vector<vector<vector<float> > > ProbabilityMap(int* strat0, int* strat1, int score0 = 0, int score1 = 0, int turn = 0, int who = 0, int initProb = 1, int trotLast = 0){


		for(int i = slice(score0, score1) + 1; i < 199; i++){
			for(int j = 0; j < queue[i].size(); j++){
				queueTurn(queue[i][j][0], queue[i][j][1], queue[i][j][2], queue[i][j][3], output[queue[i][j][0]][queue[i][j][1]][queue[i][j][2] + 10* queue[i][j][3]]);
			}

		}


	}


private:
	vector<vector<vector<int > > > queue;
	vector<vector<vector<float> > > output;

	int slice(int x, int y){
		 return x + y;
	}

	void queueTurn(int x, int y, int turnState, int trotLast, float probability){

	}
};

