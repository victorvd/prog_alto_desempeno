#include <ctime>
#include <iostream>
#include <vector>
using namespace std;

double producto(int size)
{
	vector<vector<float>> A(size, vector<float> (size, 2.3f));
	vector<vector<float>> B(size, vector<float> (size, 3.2f));
	vector<vector<float>> P(size, vector<float> (size, 0));
	
	/*vector<vector<float>> A = {{5, 3, -4, -2}, {8, -1, 0, -3}};
	vector<vector<float>> B = {{1, 4, 0}, {-5, 3, 7}, {0, -9, 5}, {5, 1, 4}};
	vector<vector<float>> P (A.size(), vector<float> (B[0].size(), 0));*/

	clock_t t;
	t = clock();
	
	for(int i = 0; i < A.size(); i++) {
		for(int j = 0; j < B[0].size(); j++){
			for(int k = 0; k < A[0].size(); k++)
				P[i][j] += A[i][k] * B[k][j];
		}
	}
	/*
	for(int i = 0; i < P.size(); i++) {
		for(int j = 0; j < P[0].size(); j++){
			cout << P[i][j] << "\t";
		}
		cout << endl;
	}
	
	cout << endl;*/

	t = clock() - t;
	
	return double(t) / CLOCKS_PER_SEC;
}

int main(void)
{
	clock_t t;
	t = clock();
	
	vector<int> sizes {200,400,600,800,1000,1200,1400,1600,1800,2000};

	cout << "\n\tExecution Time Table (seconds) \n\n";
	cout << "Dimensions\t" << "Function Delay (sec)\t\n";

	for(int i=0; i<sizes.size(); i++)
	{		
		double prod = producto(sizes[i]);
		
		printf ( "%d\t\t%.6f\n", sizes[i], prod);
	}

	cout << endl;
	
	t = clock() - t;
	
	cout << "Total time elapsed: " << double(t) / CLOCKS_PER_SEC << "\n\n";
	
	return 0;
}
