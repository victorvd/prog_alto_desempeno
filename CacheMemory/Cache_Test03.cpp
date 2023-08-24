#include <ctime>
#include <iostream>
#include <vector>
using namespace std;
#define N 6

typedef vector<float> vec;
typedef vector<vector<float>> vec2;

vec2 suma (vec2* A, vec2* B)
{
	vec2 S((*A).size(), vec((*B)[0].size(), 0));
	
	for(int i = 0; i < (*A).size(); i++)
		for(int j = 0; j < (*B)[0].size(); j++)
			S[i][j] += (*A)[i][j] + (*B)[i][j];
	return S;
}

vec2 simpleProd (vec2* A, vec2* B)
{
	vec2 P((*A).size(), vec((*B)[0].size(), 0));
	for(int i = 0; i < (*A).size(); i++) {
		for(int j = 0; j < (*B)[0].size(); j++){
			for(int k = 0; k < (*A)[0].size(); k++)
				P[i][j] += (*A)[i][k] * (*B)[k][j];
		}
	}
	return P;
}

vec2 multibloque (vec2* A, vec2* B, int n=6)
{
	if (!n){
		return simpleProd(A, B);
	}

	int mm = (*B).size();
	int m  = (*B).size()/2;

	vec2 A1((*A).size(), vec(m,0));
	vec2 A2((*A).size(), vec(mm-m,0));

	for (int i = 0; i < (*A).size(); i++)
	{
		int j=0;
		for (;j<m;j++)  A1[i][j] = (*A)[i][j];
		for (;j<mm;j++) A2[i][j-m] = (*A)[i][j];
	}

	vec2 B1(m, vec((*B)[0].size(),0));
	vec2 B2(mm-m, vec((*B)[0].size(),0));

	for (int i = 0; i < (*B)[0].size(); i++)
	{
		int j=0;
		for (;j<m;j++)  B1[j][i] = (*B)[j][i];
		for (;j<mm;j++) B2[j-m][i] = (*B)[j][i];
	}
	
	vec2 C1 = multibloque(&A1,&B1,n-1);
	vec2 C2 = multibloque(&A2,&B2,n-1);
	
	return suma (&C1, &C2);
}

double producto(int size)
{
	vec2 A(size, vec(size, 2.3f));
	vec2 B(size, vec(size, 3.2f));
	//vec2 P(size, vec(size, 0));
	
	/*vec2 A = {{5, 3, -4, -2}, {8, -1, 0, -3}};
	vec2 B = {{1, 4, 0}, {-5, 3, 7}, {0, -9, 5}, {5, 1, 4}};
	vec2 P (A.size(), vec (B[0].size(), 0));*/

	clock_t t;
	t = clock();
	
	multibloque (&A, &B, N);

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

	for(int i = 0; i < sizes.size(); i++)
	{		
		double prod = producto(sizes[i]);
		
		printf ( "%d\t\t%.6f\n", sizes[i], prod);
	}

	cout << endl;
	
	t = clock() - t;
	
	cout << "Total time elapsed: " << double(t) / CLOCKS_PER_SEC << "\n\n";
	
	return 0;
}
