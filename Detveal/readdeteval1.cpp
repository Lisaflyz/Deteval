/***************************************************************************
* readeval.cpp
*
* author: christian wolf
*         christian.wolf@insa-lyon.fr
*
* changelog:
* 24.04.2009 cw: -bugfix: no break after many files not containing text
* 19.04.2006 cw: -added the comparison between two xml files
* 27.04.2004 cw: -first version
*
* 1 tab = 4 spaces
***************************************************************************/

#define version		"version 2.01, 24.04.2009" 

// c
#include <math.h>
#include <io.h>
#include <process.h>
#include "getopt.h"
// c++
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <set>
#include <map>
#include<string>
#include<fstream>
#include<vector>
using namespace std;

// ********************************************************************
// the main function
// ********************************************************************

vector<string> split1(string str, string pattern){
	str += pattern;
	vector<string> result;
	int size = str.size();

	int pos = 0;
	for (int i = 0; i < size; i++){
		pos = str.find(pattern, i);
		if (pos < size){
			string s = str.substr(i, pos - i);
			i = pos + pattern.size() - 1;
			result.push_back(s);
		}

	}

	return result;

}

int stringToInt(string s){
	stringstream ss;
	ss << s;
	int temp;
	ss >> temp;
	ss.clear();
	return temp;
}

double stringToDouble(string s){
	stringstream ss;
	ss << s;
	double temp;
	ss >> temp;
	ss.clear();
	return temp;
}
int main(int argc, char **argv)
{
	
	
	string resultFile = "E:/project/Detveal/Detveal/totalFile.txt";
	ifstream totalResult(resultFile);
	char buffer[256];
	vector<int> GNum;
	vector<int> DNum;
	vector<double> RScore;
	vector<double> PScore;

	while (!totalResult.eof()){//未到文件末尾，则逐行读入矩形框并加入
		totalResult.getline(buffer, 256, '\n');//表示该行字符已达到256或遇到换行
		//	cout << "buffer = " << buffer << endl;
		vector<string> oneImageT = split1(buffer, " ");
		if (oneImageT.size() < 4){
			break;
		}
		//	cout << "poss.size() = " << poss.size() << endl;


		//利用stringstream将string转换为int
		GNum.push_back(stringToInt(oneImageT[0]));
		DNum.push_back(stringToInt(oneImageT[1]));
		RScore.push_back(stringToDouble(oneImageT[2]));
		PScore.push_back(stringToDouble(oneImageT[3]));


	}

	int totalGNum = 0, totalDNum = 0;
	double totalRNum = 0.0, totalPNum = 0.0;
	for (int i = 0; i < GNum.size(); i++){
		totalGNum += GNum[i];
	}
	for (int i = 0; i < DNum.size(); i++){
		totalDNum += DNum[i];
	}
	for (int i = 0; i <RScore.size(); i++){
		totalRNum += RScore[i];
	}
	for (int i = 0; i <PScore.size(); i++){
		totalPNum += PScore[i];
	}

	double Recall = totalRNum / totalGNum;
	double Precision = totalPNum / totalDNum;
	double FScore = 2 * Recall * Precision / (Recall + Precision);

	cout << "Recall = " << Recall << " Precision = " << Precision << " FScore = " << FScore <<  endl;

			


	return 0;
}

