#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <iostream>

using std::fstream;
using std::endl;
using std::cout;

#include "pam.h"

// ==================================================================================================================================================
//                                                                                                                                           PAM::PAM
// ==================================================================================================================================================
PAM::PAM(const int n_, const int k_, double* distanceMatrix_):
n(n_), k(k_),
distanceMatrix(distanceMatrix_),
iterationsCounter(0),
targetFunctionValue(0)
{
    cout << "the beginning" << endl;

    distanceMatrix_ = NULL;
    // medoidsIndexes = new int[k];
    // for (int i = 0; i < k; i++){
    //     // Random distribution (this is redundancy, because of BUILD phase of PAM algorithm)
    //     medoidsIndexes[i] = i;
    // }
}

// ==================================================================================================================================================
//                                                                                                                                          PAM::~PAM
// ==================================================================================================================================================
PAM::~PAM(){
    delete [] distanceMatrix; distanceMatrix = NULL;
}

// ==================================================================================================================================================
//                                                                                                                                    PAM::BuildPhase
// ==================================================================================================================================================
void PAM::BuildPhase (){

    double* previousMIN = new double [n];

    for (int currentMedoid_i = 0; currentMedoid_i < k; currentMedoid_i++){
        if (currentMedoid_i == 0){
            
            double minSum = 0; // target function value
            int bestNonMedoid = 0;

            bool firstAttempt = true;
            for (int i = 0; i < n; i++){
                double sum = 0;
                
                for (int j = 0; j < n; j++){
                    sum += distanceMatrix[i*n + j];
                }

                if (firstAttempt or minSum > sum) {
                    minSum = sum;
                    bestNonMedoid = i;
                    firstAttempt = false;
                }
            }

            targetFunctionValue = minSum;
            medoidsIndexes.insert(bestNonMedoid);
            for (int i = 0; i < n; i++){
                previousMIN[i] = distanceMatrix[bestNonMedoid*n + i];
            }

        } else {

            double minSum = 0;
            int bestNonMedoid = 0;

            bool firstAttempt = true;
            for (int i = 0; i < n; i++){

                // We search for new medoids only in multitude of non-medoids
                if (medoidsIndexes.find(i) != medoidsIndexes.end()){
                    continue;
                }

                // Counting sum of mins
                double sum = 0;
                for (int j = 0; j < n; j++){
                    sum += std::min(previousMIN[j], distanceMatrix[i*n + j]);
                }

                if (firstAttempt or minSum > sum) {
                    minSum = sum;
                    bestNonMedoid = i;
                    firstAttempt = false;
                }
            }

            targetFunctionValue = minSum;
            medoidsIndexes.insert(bestNonMedoid);
            for (int i = 0; i < n; i++){
                previousMIN[i] = std::min(previousMIN[i], distanceMatrix[bestNonMedoid*n + i]);
            }

        }
    }

    delete [] previousMIN;
}

// ==================================================================================================================================================
//                                                                                                                                     PAM::SwapPhase
// ==================================================================================================================================================
void PAM::SwapPhase (const ProcParams& procParams, const int itMax){

    if (itMax != 0){
        return;
    }
}

// ==================================================================================================================================================
//                                                                                                                                          PAM::Dump
// ==================================================================================================================================================
void PAM::Dump (const string& fout_str) const{
    fstream fout(fout_str, fstream::out | fstream::app);

    fout << "n= " << n << endl;
    fout << "k= " << k << endl;
    fout << "iterationsCounter= " << iterationsCounter << endl;
    fout << "targetFunctionValue= " << targetFunctionValue << endl;

    fout << "medoidsIndexes:" << endl;
    for (auto it = medoidsIndexes.begin(); it != medoidsIndexes.end(); it++){
        fout << *it << " ";
    }
    fout << endl;

    fout << "distanceMatrix:" << endl << std::fixed;
    for (int i = 0; i < n; i++){
        for (int j = 0; j < n; j++){
            fout << std::setprecision(5) << distanceMatrix[i*n + j] << " ";
        }
        fout << endl;
    }

    fout.close();
}
