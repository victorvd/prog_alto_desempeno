#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#undef DEBUG

#ifdef DEBUG
const int RMAX = 100;
#else
const int RMAX = 10000000;
#endif

int thread_count = 4;

void Generate_list(int a[], int n);
void Print_list(int a[], int n, char* title);
void Odd_evenA(int a[], int n);
void Odd_evenB(int a[], int n);

/*-----------------------------------------------------------------*/
int main() {
	int n;
	//char g_i;
	int* a;
	double startA, finishA;
	double startB, finishB;

	#ifdef DEBUG
	int sizes [] = {16};
	#else
	int sizes [] = {4096, 8192, 16384, 32768, 65536, 131072};
	printf("\n\tExecution Time Table (seconds) \n\n");
	printf("Order of Matrix\t\tOddEven1 (sec)\t\tOddEven2 (sec)\t\n");
	#endif

        for(int i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++)
        {
		n = sizes[i];
		a = malloc(n*sizeof(int));
		
		Generate_list(a, n);
		#ifdef DEBUG
		Print_list(a, n, "Before sort");
		#endif

		startA = omp_get_wtime();
		Odd_evenA(a, n);
		finishA = omp_get_wtime();
		
		startB = omp_get_wtime();
		Odd_evenB(a, n);
		finishB = omp_get_wtime();

		#ifdef DEBUG
		Print_list(a, n, "After sort");
		#endif
		
		printf ("%d\t\t\t%.6f\t\t%.6f\n", n, finishA - startA, finishB - startB);

		free(a);
	}
	return 0;
}  /* main */


void Generate_list(int a[], int n) {
	int i;

	srandom(1);
	for (i = 0; i < n; i++)
		a[i] = random() % RMAX;
}  /* Generate_list */


void Print_list(int a[], int n, char* title) {
	int i;

	printf("%s:\n", title);
	for (i = 0; i < n; i++)
		printf("%d ", a[i]);
	printf("\n\n");
}  /* Print_list */


void Odd_evenA(int a[], int n) {
	int phase, i, tmp;
#ifdef DEBUG
	char title[100];
#endif

	for (phase = 0; phase < n; phase++) {
		if (phase % 2 == 0)
#pragma omp parallel for num_threads(thread_count) \
				default(none) shared(a, n) private(i, tmp)
			for (i = 1; i < n; i += 2) {
				if (a[i-1] > a[i]) {
					tmp = a[i-1];
					a[i-1] = a[i];
					a[i] = tmp;
				}
			}
		else
#pragma omp parallel for num_threads(thread_count) \
				default(none) shared(a, n) private(i, tmp)
			for (i = 1; i < n-1; i += 2) {
				if (a[i] > a[i+1]) {
					tmp = a[i+1];
					a[i+1] = a[i];
					a[i] = tmp;
				}
			}
#ifdef DEBUG
		sprintf(title, "After phase %d", phase);
		Print_list(a, n, title);
#endif
	}
}  /* Odd_even */

void Odd_evenB(int a[], int n) {
	int phase, i, tmp;

#pragma omp parallel num_threads(thread_count) \
		default(none) shared(a, n) private(i, tmp, phase)
	for (phase = 0; phase < n; phase++) {
		if (phase % 2 == 0)
#pragma omp for 
			for (i = 1; i < n; i += 2) {
				if (a[i-1] > a[i]) {
					tmp = a[i-1];
					a[i-1] = a[i];
					a[i] = tmp;
				}
			}
		else
#pragma omp for 
			for (i = 1; i < n-1; i += 2) {
				if (a[i] > a[i+1]) {
					tmp = a[i+1];
					a[i+1] = a[i];
					a[i] = tmp;
				}
			}
	}
}  /* Odd_even */
