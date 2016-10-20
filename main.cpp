#include <mpi.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
// #include <ctime>
#include <chrono>
// #include <algorithm>
#include <set>

#include "pam.h"

using std::string;
using std::fstream;
using std::cout;
using std::endl;
using std::set;
using std::istringstream;
// using std::clock;
// using std::clock_t;

// ==================================================================================================================================================
//                                                                                                                                          InputData
// ==================================================================================================================================================
struct InputData {

    unsigned const int n; // Size of multitude (vectors in R^m)
    unsigned const int m; // vector dimension (vector is point in R^m)
    double* vectors;

    InputData(const unsigned int n, const unsigned int m, const string& fin_str);
    ~InputData();

    // FUTURE WORK: Calculations of distance matrix can be parallelized
    double* GenerateDistanceMatrix() const;
};

// ==================================================================================================================================================
//                                                                                                                                               MAIN
// ==================================================================================================================================================
int main(int argc, char* argv[]){

    // Input checking

    if (argc != 7){
        cout << "Wrong parameters. Usage: './binary n m k input_data.txt output_data.txt t'" << endl;
        // argv[0] - pointer to binary name
        // argv[1] - pointer to number n - multitude power
        // argv[2] - pointer to number m - vector length
        // argv[3] - pointer to number k - clusters number
        // argv[4] - pointer to output data
        // argv[5] - pointer to input data
        // argv[6] - pointer to number t - number of iterations to compute for time measurement
        return 0;
    }

    unsigned int n; istringstream(argv[1]) >> n;
    unsigned int m; istringstream(argv[2]) >> m;
    unsigned int k; istringstream(argv[3]) >> k;
    string input_file = string(argv[4]);
    string output_file = string(argv[5]);
    unsigned int t; istringstream(argv[6]) >> t;

    // ================================
    MPI_Init(&argc,&argv); // Start MPI
    // ================================

    ProcParams procParams;

    InputData inputData (n, m, input_file);

    double* distanceMatrix = inputData.GenerateDistanceMatrix();

    PAM pam (n, k, distanceMatrix);

    inputData.~InputData();

    // pam.BuildPhaseConsecutive();

    auto startClock = std::chrono::high_resolution_clock::now();

    pam.BuildPhaseParallel(procParams);
    pam.SwapPhaseParallel(procParams, t);

    auto finishClock = std::chrono::high_resolution_clock::now();

    // Output results
    if (procParams.rank == 0){

        // double PAMduration = ( clock() - startClock ) / (double) CLOCKS_PER_SEC;

        fstream fout (output_file, fstream::out | fstream::app);
        fout << "n= " << n << " m= " << m << " k= " << k << " t= " << t << endl;
        fout << "timeDuration= " << std::chrono::duration_cast<std::chrono::nanoseconds>(finishClock - startClock).count() << " nano-seconds = "
             << std::chrono::duration_cast<std::chrono::milliseconds>(finishClock - startClock).count() << " milli-seconds" << endl;
        fout << "iterations= " << pam.getIterationsCounter() << endl;
        fout << "targetFunctionValue= " << pam.getTargetFunctionValue() << endl;
        fout << "Medoids(points-indexes):" << endl;

        set<unsigned int> medoidsIndexes = pam.getMedoids();
        for (auto it = medoidsIndexes.begin(); it != medoidsIndexes.end(); it++){
            fout << *it << " ";
        }
        fout << endl;
        fout.close();
    }

    delete [] distanceMatrix; distanceMatrix = NULL;

    // ========================
    MPI_Finalize(); // Stop MPI
    // ========================

    return 0;
}

// ==================================================================================================================================================
//                                                                                                                               InputData::InputData
// ==================================================================================================================================================
InputData::InputData(const unsigned int n_, const unsigned int m_, const string& fin_str):
n(n_), m(m_)
{
    fstream fin(fin_str.c_str(), fstream::in);

    vectors = new double [n*m];
    for (unsigned int i = 0; i < n; i++){
        for (unsigned int j = 0; j < m; j++){
            fin >> vectors[i*m + j];
        }
    }

    fin.close();
}

// ==================================================================================================================================================
//                                                                                                                              InputData::~InputData
// ==================================================================================================================================================
InputData::~InputData() {
    delete [] vectors; vectors = NULL;
}

// ==================================================================================================================================================
//                                                                                                                  InputData::GenerateDistanceMatrix
// ==================================================================================================================================================
double* InputData::GenerateDistanceMatrix() const{

    double* distanceMatrix = new double [n*n];

    for (unsigned int i = 0; i < n; i++){
        distanceMatrix[i*n + i] = 0;
        for (unsigned int j = i+1; j < n; j++){
            double rho = 0;
            for (unsigned int k = 0; k < m; k++){
                double diff = vectors[i*m + k] - vectors[j*m + k];
                rho += diff * diff;
            }
            distanceMatrix[i*n + j] = distanceMatrix[j*n + i] = std::sqrt(rho);
        }
    }

    return distanceMatrix;
}
