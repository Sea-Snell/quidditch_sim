
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
using namespace std;
class DataManager
{
private:
	vector<string> fileStrings;
public:
	const char* fileName;
	vector< vector <int> > stratData;
	ofstream datawriter;
	ifstream datareader;

	DataManager(const char* fileName){
		this->fileName = fileName;
		DataGrab();
		ParseData();

	}

	void DataGrab(){
		datareader.open(fileName);
		string tempStr;
		fileStrings.clear();
		while(getline(datareader, tempStr)){
			fileStrings.push_back(tempStr);
		}
		datareader.close();
	}

	void ParseData(){
		stratData.clear();
		for(int i = 1; i < fileStrings.size(); i += 2){
			vector<int> tempIntVec;
			string tempString = "";
			for(int j = 1	; j < fileStrings[i].size(); j += 1){
				if(fileStrings[i][j] == ','){
					tempIntVec.push_back(stoi(tempString));
					tempString = "";
					j++;
					cout << tempString << endl;
				}
				else{
				tempString = tempString + fileStrings[i][j];
				}
			}
			cout << tempIntVec[3] << endl;
			stratData.push_back(tempIntVec);
		}
	}

	void ReadData(){

	}

	void AddStrat(const char* stratName, int stratData[10000]){
		datawriter.open(fileName, fstream::app);
		datawriter << stratName << "\n[";
		for(int i = 0; i < 9999; i++){
			datawriter << stratData[i];
			datawriter << " , ";
		}
		datawriter << stratData[9999] << "]\n";
		datawriter.close();
	}
};

