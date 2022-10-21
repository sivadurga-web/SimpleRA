#include <bits/stdc++.h>
using namespace std;

int main() {
	vector<int> a(100);
	for (int i = 1 ; i <= 100;i++) a[i-1] = i;
	unsigned seed = 0;
	random_shuffle(a.begin(), a.end());
	for (int i = 0 ; i < 100;i++) {
		cout << "INSERT "<<a[i]<<endl;
	}
}
