#include <iostream>
#include <string>
#include <vector>
#include <bitset>
#include <fstream>
#include <sstream>
#include <cmath>
#include <ctime>
#include "helper.h"
using namespace std;
#define M 35 // number of columns in input matrix

const int MIN_PATTERN_SIZE = 10;
const double SEED_SIM_THR = 0.7;
int minsup;
double noise, simThr; // simThr: pattern similarity threshold

void readMatrix(string fileName, vector<bitset<M> >& mat) {
  string line;
  ifstream ifs(fileName);
  while (getline(ifs, line)) {
    bitset<M> row(line);
    mat.push_back(row);
  }
  ifs.close();
}

void computeValidRows(vector<bitset<M> >& mat, vector<int>& validRows) {
  int minSize = int(round((1-noise)*minsup)); // min number of ones per row
  int n = mat.size();
  for (int i=0; i<n; ++i) {
    if (mat[i].count() >= minSize) validRows.push_back(i);
  }
}

// Based on Jaccard similarity
bool isSimilar(bitset<M>& s1, bitset<M>& s2) {
  int n1 = s1.count();
  int n2 = s2.count();
  bitset<M> intersection = s1 & s2;
  int overlap = intersection.count();
  double sim = ((double) overlap) / (n1 + n2 - overlap);
  return sim >= SEED_SIM_THR;
}

// Based on Jaccard similarity. Assumes vectors are sorted in ascending order.
bool isSimilar(vector<int>& p1, vector<int>& p2) {
  int n1 = max(p1.size(), p2.size());
  int n2 = min(p1.size(), p2.size());
  if (((double) n2) / n1 < simThr) return false;

  int x = 0; // intersection count
  int y = 0; // non-intersection count
  // If x > maxX, vectors are similar. If y > maxY, vectors are NOT similar. (See derivation)
  int maxX = int(floor(simThr*(n1+n2)/(simThr+1)));
  int maxY = int(floor((n1+n2)*(1-simThr)/(1+simThr)));

  vector<int>::iterator it1 = p1.begin();
  vector<int>::iterator it2 = p2.begin();
  while(true) {
    if (it1==p1.end() || it2==p2.end()) break; // finished counting x
    if (*it1 == *it2) {
      x++;
      if (x > maxX) return true;
	  it1++;
	  it2++;
    } else {
      y++;
      if (y > maxY) return false;
      if (*it1 > *it2) it2++; else it1++;
    }
  }
  return x > maxX;
}

// Check if seed has been visited
bool isVisited(bitset<M>& seed, vector<bitset<M> >& visitedSeeds) {
  vector<bitset<M> >::iterator it;
  for (it=visitedSeeds.begin(); it!=visitedSeeds.end(); ++it) {
    if (isSimilar(seed, *it)) return true;
  }
  return false;
}

// Check if pattern has been visited
bool isVisited(vector<int>& pattern, vector<vector<int> >& finalPatterns) {
  vector<vector<int> >::iterator it;
  for (it=finalPatterns.begin(); it!=finalPatterns.end(); ++it) {
    if (isSimilar(pattern, *it)) return true;
  }
  return false;
}

void run(vector<bitset<M> >& mat, 
        vector<int>& validRows, 
        vector<bitset<M> >& visitedSeeds, 
        vector<vector<int> >& finalPatterns, 
        vector<bitset<M> >& finalSeeds) {
  int n = validRows.size();
  int r1, r2, r3, seedSize, minSize;
  bitset<M> seed;
  for (int i=0; i<n-1; ++i) {
    r1 = validRows[i];
    if (i % 1000 == 0) cout << "Working on row " << r1 << endl;
    for (int j=i+1; j<n; ++j) {      
      r2 = validRows[j];
      seed = mat[r1] & mat[r2];
      seedSize = seed.count();

      if (seedSize < minsup) continue;
      if (isVisited(seed, visitedSeeds)) continue;
      
      visitedSeeds.push_back(seed);
      vector<int> pattern;
      pattern.reserve(n);
      pattern.push_back(r1);
      pattern.push_back(r2);
      
      minSize = int(round((1-noise)*seedSize)); // min number of ones per row
      for (int k=j+1; k<n; ++k) {
        r3 = validRows[k];
        bitset<M> overlap = seed & mat[r3];
        if (overlap.count() >= minSize) pattern.push_back(r3);
      }
      if (pattern.size() < MIN_PATTERN_SIZE) continue;
      if (!isVisited(pattern, finalPatterns)) {
        finalPatterns.push_back(pattern);
        finalSeeds.push_back(seed);
      }
    }
  }
}

string generateStatsString(char** argv, 
                vector<bitset<M> >& mat, 
                vector<int>& validRows, 
                vector<vector<int> >& finalPatterns,
                double meanPatternSize,
                double runtime) {
  ostringstream oss;
  oss << "input file: " << argv[1] << endl
      << "output file: " << argv[2] << endl
      << "minsup: " << minsup << endl
      << "noise: " << noise << endl
      << "simThr: " << simThr << endl
      << "rows: " << mat.size() << endl
      << "valid rows: " << validRows.size() << endl
      << "final patterns: " << finalPatterns.size() << endl
      << "average pattern size: " << meanPatternSize << endl
      << "runtime(s): " << runtime << endl;
  return oss.str();
}

// Each line contains list of row ids in pattern and corresponding seed bitset. Row ids are 1-indexed.
void outputPatternsToFile(vector<vector<int> >& finalPatterns, vector<bitset<M> >& finalSeeds, string outFileName) {
  ofstream ofs(outFileName);
  int n = finalPatterns.size();
  for (int i=0; i<n; ++i) {
    vector<int>::iterator it;
    for (it=finalPatterns[i].begin(); it!=finalPatterns[i].end(); ++it) {
      ofs << *it+1 << " "; // 1-indexing
    }
    ofs << finalSeeds[i].to_string() << endl;
  }
  ofs.close();
}

void outputStringToFile(string s, string outFileName) {
  ofstream ofs(outFileName);
  ofs << s;
  ofs.close();
}

int main(int argc, char** argv) {
  if (argc < 6) {
    cout << "./bibitn matrixFile outputFile minsup noise simThr" << endl;
    return 1; 
  }
  string matrixFileName = argv[1];
  string outFileName = argv[2];
  minsup = atoi(argv[3]);
  noise = atof(argv[4]);
  simThr = atof(argv[5]);

  vector<bitset<M> > mat;
  vector<int> validRows;
  vector<bitset<M> > visitedSeeds;
  vector<vector<int> > finalPatterns;
  vector<bitset<M> > finalSeeds;
  
  clock_t begin = clock();
  cout << "Reading data..." << endl;
  readMatrix(matrixFileName, mat);
  cout << "rows: " << mat.size() << endl;
  computeValidRows(mat, validRows);
  cout << "valid rows: " << validRows.size() << endl;
  run(mat, validRows, visitedSeeds, finalPatterns, finalSeeds);
  cout << "Done." << endl;
  clock_t end = clock();

  double meanPatternSize = meanSize(finalPatterns);
  double runtime = double(end-begin)/CLOCKS_PER_SEC;
  string stats = generateStatsString(argv, mat, validRows, finalPatterns, meanPatternSize, runtime);
  cout << stats;

  outputPatternsToFile(finalPatterns, finalSeeds, outFileName);
  outputStringToFile(stats, outFileName + "_stats");

  return 0;
}
