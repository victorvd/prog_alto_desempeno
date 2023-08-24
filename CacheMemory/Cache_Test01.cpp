#include <ctime>
#include <iostream>
#include <vector>
using namespace std;

double loopij(int size)
{
	vector<vector<float>> A(size, vector<float> (size, 2.3f));
	vector<float> x(size, 3.2f), y(size, 0);

	clock_t t;
        t = clock();
	
        for(int i = 0; i < size; i++)
        {
            for(int j = 0; j < size; j++)
                y[i] += A[i][j]*x[j];
        }

        t = clock() - t;
        
        return double(t) / CLOCKS_PER_SEC;
}

double loopji(int size)
{
	vector<vector<float>> A(size, vector<float> (size, 2.3f));
	vector<float> x(size, 3.2f), y(size, 0);
        
       	clock_t t;
        t = clock();
	
        for(int j = 0; j < size; j++)
        {
            for(int i = 0; i < size; i++)
                y[i] += A[i][j]*x[j];
        }

        t = clock() - t;
        
        return double(t) / CLOCKS_PER_SEC;
}

int main(void)
{
       	clock_t t;
        t = clock();
        
	vector<int> sizes {50,100,250,500,1000,2000,4000,8000,16000,32000};

	cout << "\n\tExecution Time Table (seconds) \n\n";
	cout << "Dimensions\t" << "Loop ij (sec)\t" << "Loop ji (sec)\t" << "Ratio\t\n";

        for(int i=0; i<sizes.size(); i++)
        {		
		double ji = loopji(sizes[i]);
		double ij = loopij(sizes[i]);
		
		printf ( "%d\t\t%.6f\t%.6f\t%.3f\n", sizes[i], ij, ji, ji/ij);
	}

	cout << endl;
	
        t = clock() - t;
        
        cout << "Total time elapsed: " << double(t) / CLOCKS_PER_SEC << "\n\n";
        
	return 0;
}
