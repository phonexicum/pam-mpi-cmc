#include <mpi.h>

#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
// #include <chrono>
// #include <algorithm>
#include <vector>
#include <set>
#include <utility>

#include <sys/time.h>

#include "pam.h"

using std::string;
using std::fstream;
using std::cout;
using std::cerr;
using std::endl;
using std::set;
using std::istringstream;
using std::vector;

// ==================================================================================================================================================
//                                                                                                                                          InputData
// ==================================================================================================================================================
struct InputData {

    unsigned const int n; // Size of multitude (vectors in R^m)
    unsigned const int m; // vector dimension (vector is point in R^m)
    double* vectors;

    InputData(const unsigned int n, const unsigned int m, const string& fin_str);
    ~InputData();

    double* GenerateDistanceMatrix() const;
};

// ==================================================================================================================================================
//                                                                                                                                               MAIN
// ==================================================================================================================================================

void Benchmark (const int n, const int k, const int p, const int m, const int maxIt, const string fin_name, const string fout_name){

    MPI_Barrier (MPI_COMM_WORLD);

    ProcParams procParams;

    if (procParams.rank == 0){
        cout << "Benchmark with parameters: " << n << " " << p << " " << maxIt << endl;
    }

    // MPI initialization
    // 
    if (procParams.size < p) {
        cout << "Error. number of processes '" << procParams.size << "' is less then required= " << p << endl;
        return;
    }

    MPI_Comm currentComm;
    if (procParams.rank < p){
        MPI_Comm_split(MPI_COMM_WORLD, 1, procParams.rank, &currentComm);
    } else {
        MPI_Comm_split(MPI_COMM_WORLD, MPI_UNDEFINED, procParams.rank, &currentComm);
    }

    if (currentComm != MPI_COMM_NULL){

        procParams = ProcParams(currentComm);

        // algorithm initialization
        // 
        InputData inputData (n, m, fin_name);
        double* distanceMatrix = inputData.GenerateDistanceMatrix();
        PAM pam (n, k, distanceMatrix);
        inputData.~InputData();

        // algorithm run
        // 
        MPI_Barrier (currentComm);

        struct timeval tp;
        gettimeofday(&tp, NULL);
        long int startClock = tp.tv_sec * 1000 + tp.tv_usec / 1000; // milliseconds
        long int medianClock;
        // auto startClock = std::chrono::high_resolution_clock::now();
        // auto medianClock = startClock;

        if (procParams.rank < procParams.size){

            pam.BuildPhaseParallel(procParams);

            gettimeofday(&tp, NULL);
            medianClock = tp.tv_sec * 1000 + tp.tv_usec / 1000;
            // medianClock = std::chrono::high_resolution_clock::now();
            
            pam.SwapPhaseParallel(procParams, maxIt);

        }
        gettimeofday(&tp, NULL);
        long int finishClock = tp.tv_sec * 1000 + tp.tv_usec / 1000;
        // auto finishClock = std::chrono::high_resolution_clock::now();
        MPI_Barrier (currentComm);


        delete [] distanceMatrix; distanceMatrix = NULL;

        // algorithm results
        if (procParams.rank == 0){

            fstream fout (fout_name.c_str(), fstream::out | fstream::app);
            fout << "n= " << n << " m= " << m << " k= " << k << " p= " << p << endl;

            // fout << "buildTimeDuration= " << std::chrono::duration_cast<std::chrono::nanoseconds>(medianClock - startClock).count() << " nano-seconds" << endl;
            // fout << "swapTimeDuration= " << std::chrono::duration_cast<std::chrono::nanoseconds>(finishClock - medianClock).count() << " nano-seconds" << endl;
            // fout << "overalTimeDuration= " << std::chrono::duration_cast<std::chrono::nanoseconds>(finishClock - startClock).count() << " nano-seconds" << endl;
            fout << "buildTimeDuration= " << medianClock - startClock << " milli-seconds" << endl;
            fout << "swapTimeDuration= " << finishClock - medianClock << " milli-seconds" << endl;
            fout << "overalTimeDuration= " << finishClock - startClock << " milli-seconds" << endl;

            fout << "iterations= " << pam.getIterationsCounter() << endl;
            // fout << "targetFunctionValue= " << pam.getTargetFunctionValue() << endl;
            // fout << "Medoids(points-indexes):" << endl;

            // set<unsigned int> medoidsIndexes = pam.getMedoids();
            // for (auto it = medoidsIndexes.begin(); it != medoidsIndexes.end(); it++){
            //     fout << *it << " ";
            // }
            fout << endl;
            fout.close();
        }

        // MPI free
        MPI_Comm_free (&currentComm);
    }
}

// ==================================================================================================================================================
//                                                                                                                                               MAIN
// ==================================================================================================================================================
int main(int argc, char* argv[]){

    // Input checking

    if (argc != 4){
        // cout << "Wrong parameters. Usage: './binary n m k input_data.txt output_data.txt t'" << endl;
        cout << "Wrong parameters. Usage: './binary settings.txt input_data.txt output_data.txt'" << endl;
        // argv[0] - pointer to binary name
        // argv[1] - pointer to number n - multitude power
        // argv[2] - pointer to number m - vector length
        // argv[3] - pointer to number k - clusters number
        // argv[4] - pointer to output data
        // argv[5] - pointer to input data
        // argv[6] - pointer to number t - number of iterations to compute for time measurement
        return 0;
    }

    // unsigned int n; istringstream(argv[1]) >> n;
    // unsigned int m; istringstream(argv[2]) >> m;
    // unsigned int k; istringstream(argv[3]) >> k;
    // string input_file = string(argv[4]);
    // string output_file = string(argv[5]);
    // unsigned int t; istringstream(argv[6]) >> t;

    string settings_file = string(argv[1]);
    string input_file = string(argv[2]);
    string output_file = string(argv[3]);

    // ================================
    MPI_Init(&argc,&argv); // Start MPI
    // ================================

    fstream settings_fin (settings_file.c_str(), fstream::in);
    vector<int> matrix_size;
    vector<int> proc_num;
    vector<int> maxIt;

    while (true) {
        int matrix, p, maxIter;
        settings_fin >> matrix >> p >> maxIter;
        matrix_size.push_back(matrix);
        proc_num.push_back(p);
        maxIt.push_back(maxIter);
        if (settings_fin.eof() == true)
            break;
    }
    settings_fin.close();

    for (unsigned int i = 0; i < matrix_size.size(); i++){
        Benchmark (matrix_size[i], matrix_size[i] / 50, proc_num[i], 5, maxIt[i], input_file, output_file);
    }

    MPI_Barrier (MPI_COMM_WORLD);

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
