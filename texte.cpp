#include <iostream>
using namespace std;

int main(){
  int var;
  cin >> var;
  cout << var << endl;
  cout << var/300 << endl;
  var = var % 300;
  cout << var/60 << endl;
  cout << var%60 << endl;
  return 0;
}