#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
using namespace std;
#define M 8

void readMatrix(string fileName, vector<bitset<M> >& mat) {
  string line;
  ifstream ifs(fileName.c_str());
  while (getline(ifs, line)) {
    bitset<M> row(line);
    mat.push_back(row);
  }
  ifs.close();
}

vector<int> computeValidRows(vector<bitset<M> >& mat, minsup) {

}

int main(int argc, char** argv) {
  if (argc < 5) {
    cout << "./bibitn matrixFile outputFile minsup noise" << endl;
    return 1; 
  }

  string matrixFileName = argv[1];
  string outFileName = argv[2];
  int minsup = atoi(argv[3]);
  double noise = atof(argv[4]);

  vector<bitset<M> > mat;
  readMatrix(matrixFileName, mat);
  cout << "row count = " << mat.size() << endl;

  return 0;
}
