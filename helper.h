#include <iostream>
#include <string>
#include <vector>
using namespace std;

void printVector(vector<int>& v) {
  vector<int>::iterator it;
  for (it=v.begin(); it!=v.end(); ++it) cout << *it << " ";
  cout << endl;
}

void print2dVector(vector<vector<int> >& v) {
  vector<vector<int> >::iterator it;
  for (it=v.begin(); it!=v.end(); ++it) printVector(*it);
}

double meanSize(vector<vector<int> >& v) {
  int sum = 0;
  vector<vector<int> >::iterator it;
  for (it=v.begin(); it!=v.end(); ++it) sum += (*it).size();
  return ((double) sum) / v.size();
}
