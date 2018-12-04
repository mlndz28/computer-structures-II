#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <pthread.h>
#include <stdint.h>

using namespace std;

int checkPi(double pi);
void machin(int iterations);
void printUsage();
int iterations = 10;
int threads = -1;
double pi = 0;
vector<pthread_t> thread_id;
vector<pthread_mutex_t> mutexes;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

int main (int argc, char *argv[]){
	int i = 1;
	while (i < argc && argv != nullptr) {
    	if (string(argv[i]) == "-i") {
      		iterations = atoi(argv[i+1]);
    	} else if (string(argv[i]) == "-t") {
      		threads = atoi(argv[i+1]);
    	} else {
      		printUsage();
    	}
		i+=2;
  	}
	if(threads < 0){
		printUsage();
	}
	thread_id.resize(threads);
	mutexes.resize(threads);
	for (int i = 0; i < threads; i++) pthread_mutex_init(&mutexes[i], NULL); // initialize all the mutexes in the array
	cout << "Calculating pi with series of " << iterations << " elements, using " << threads << " threads." << endl;
	machin(iterations);
	int precision = checkPi(pi);
	cout << "Pi: " << setprecision(precision) << pi << endl;
	cout << precision << " digits of precision" << endl;
}

// Each iteration adds two values of the overall series to pi.
void *machinIteration(void *iptr) {
    int i = (long)iptr;
	long double subSeries = 16*(i%2?-1:1)*pow(0.2, 2*i+1)/(2*i+1) - 4*(i%2?-1:1)*pow(1.0/239, 2*i+1)/(2*i+1);
	pthread_mutex_lock( &(mutexes[i%threads]) );	// lock the thread so no one can access to pi while at it
   	pi += subSeries;
   	pthread_mutex_unlock( &(mutexes[i%threads]) );
    pthread_exit(NULL);	// finish this thread's work
 }

// Calculates pi using Machin's formula. It's good enough for < 15 precision digits.
void machin(int iterations){
	for(int i = 0; i < iterations; i += threads){
		for(int j = 0; j < threads && (i+j) < iterations; j++){	// use series of threads to assign work
			int n = i + j;
			pthread_create( &thread_id[j], NULL, machinIteration, (void *)(intptr_t)n );
		}
		for(int j = 0; j < threads && (i+j) < iterations; j++){	// wait for the series to finish its job
			pthread_join( thread_id[j], NULL);
		}
	}
}

// return the amount of matching digits
int checkPi(double pi){
	// pi values retrieved from internet
	const string piStr = "3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117067982148086513282306647093844609550582231725359408128481117450284102701938521105559644622948954930381964428810975665933446128475648233";
	int digits = 0;
	stringstream temp;
	temp << setprecision(50) << pi;
	string tempStr = temp.str();
	while(tempStr[digits]==piStr[digits]){
		digits++;
	}
	return digits > 1 ? digits - 1 : digits;
}

// Prints the usage chart
void printUsage(){
	string usage = "\nUsage: pi [options]";
	usage += "\n\nFlags:";
	usage += "\n\t-i\t\tIterations of the algorithm \n\t\t\t(more means a more precise result)\n";
	usage += "\n\t-t\t\tAmount of threads to be used\n";
	cout << usage << endl;
	exit(1);
}
