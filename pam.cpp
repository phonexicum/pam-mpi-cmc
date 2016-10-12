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
//                                                                                                                         PAM::BuildPhaseConsecutive
// ==================================================================================================================================================
void PAM::BuildPhaseConsecutive (){

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

    // computation of task's positions for current process from calculation graph
    
    int tasksNum = k*(n-k);
    int tasksPerProcess = tasksNum / procParams.size + (tasksNum % procParams.size != 0);
    int freeTaskPlaces = tasksPerProcess - tasksNum % procParams.size;
    if (freeTaskPlaces == tasksPerProcess) freeTaskPlaces = 0;
    int fullyLoadedProcesses = procParams.size - freeTaskPlaces;

    int absolutePosition = 0;
    int tasksPerThisProcess = 0;
    if (procParams.rank < fullyLoadedProcesses){
        absolutePosition = procParams.rank * tasksPerProcess;
        tasksPerThisProcess = tasksPerProcess;
    } else {
        absolutePosition = fullyLoadedProcesses * tasksPerProcess + (procParams.rank - fullyLoadedProcesses) * (tasksPerProcess-1);
        tasksPerThisProcess = tasksPerProcess -1;
    }
    int replacedMedoidIndex = absolutePosition / (n-k);
    int potentialNonMedoidIndex = absolutePosition % (n-k);

    // in result:
    // 
    // tasksPerThisProcess
    // replacedMedoidIndex
    // potentialNonMedoidIndex
    // 

    double* min_m_s = new double [n];

    while (true){

        // Calculation of best minSum and arguments m_s and o_h

        double minSum = 0;
        int minSum_m_s = 0;
        int minSum_o_h = 0;

        bool firstTimeCalculation = true;
        int m_s = replacedMedoidIndex;
        bool m_s_changed = true;
        int o_h = potentialNonMedoidIndex;
        for (int computedTasks = 0; computedTasks < tasksPerThisProcess; computedTasks++, o_h++){
            if (medoidsIndexes.find(o_h) == medoidsIndexes.end()){

                if (m_s_changed == true)
                {
                    // Computing independent from summ and from o_h changing minimum
                    //      (computing min_m_s)
                    for (int i = 0; i < n; i++){
                        int it = medoidsIndexes.begin();
                        if (*it == m_s) it++;
                        min_m_s[i] = it; it++;
                        for (; it != medoidsIndexes.end(); it++){
                            if (*it != m_s){
                                min_m_s[i] = std::min(min_m_s[i], distanceMatrix[m_s*n + i]);
                            }
                        }
                    }
                    m_s_changed = false;
                }

                double sum = 0;
                // Computing sum(min(a,b))
                for (int i = 0; i < n; i++){
                    sum += std::min(min_m_s[i], distanceMatrix[o_h*n + i]);
                }
                if (sum < minSum or firstTimeCalculation){
                    minSum = sum;
                    minSum_m_s = m_s;
                    minSum_o_h = o_h;
                    firstTimeCalculation = false;
                }
            }
            if (o_h >= n-k){
                o_h = 0;
                m_s++;
                m_s_changed = true;
            }
            if (m_s >= k){
                cout << "Fatal error. This situation, when 'm_s >= k' must never rise." << endl;
            }
        }

        // Send minSum

        

        if (procParams.rank < k){
            // Receive minimum and synchronize
            
        }

        iterationsCounter ++;
        if (itMax != 0 && iterationsCounter < itMax ){
            break;
        }
    }
}

// ==================================================================================================================================================
//                                                                                                                                          PAM::Dump
// ==================================================================================================================================================
void PAM::Dump (const string& fout_str) const{
    
    fstream fout(fout_str, fstream::out | fstream::app);
    fout << "========== Dump begin ==========" << endl;

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

    fout << "========== Dump end ==========" << endl;
    fout.close();
}
