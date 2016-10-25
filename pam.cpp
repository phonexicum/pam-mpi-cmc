#include <iostream>
#include <fstream>
#include <cmath>
#include <iomanip>
#include <chrono>
#include <iostream>

#include <unistd.h>

using std::fstream;
using std::endl;
using std::cout;

#include "pam.h"

// ==================================================================================================================================================
//                                                                                                                                           PAM::PAM
// ==================================================================================================================================================
PAM::PAM(const unsigned int n_, const unsigned int k_, double* distanceMatrix_):
n(n_), k(k_),
distanceMatrix(distanceMatrix_),
iterationsCounter(0),
targetFunctionValue(0)
{
    for (unsigned int i = 0; i < n; i++)
        nonMedoidsIndexes.insert(i);
}

// ==================================================================================================================================================
//                                                                                                                                          PAM::~PAM
// ==================================================================================================================================================
PAM::~PAM(){
    distanceMatrix = NULL;
    // delete [] distanceMatrix; distanceMatrix = NULL;
}

// ==================================================================================================================================================
//                                                                                                                         PAM::BuildPhaseConsecutive
// ==================================================================================================================================================
void PAM::BuildPhaseConsecutive (){

    // fstream("output.txt", fstream::out | fstream::app) << "BuildPhaseConsecutive" << endl;

    if (not CheckParametersCorrectness())
        return;

    double* previousMIN = new double [n];


    for (unsigned int currentMedoid_i = 0; currentMedoid_i < k; currentMedoid_i++){
        if (currentMedoid_i == 0){
            
            double minSum = 0; // target function value
            set<unsigned int>::iterator bestNonMedoid;

            bool firstAttempt = true;
            for (auto it = nonMedoidsIndexes.begin(); it != nonMedoidsIndexes.end(); it++){
                double sum = 0;
                
                for (unsigned int j = 0; j < n; j++){
                    sum += distanceMatrix[(*it)*n + j];
                }

                if (firstAttempt or minSum > sum) {
                    minSum = sum;
                    bestNonMedoid = it;
                    firstAttempt = false;
                }
            }

            targetFunctionValue = minSum;
            unsigned int bestNM = *bestNonMedoid;
            medoidsIndexes.insert(bestNM);
            nonMedoidsIndexes.erase(bestNonMedoid);
            for (unsigned int i = 0; i < n; i++){
                previousMIN[i] = distanceMatrix[bestNM*n + i];
            }

        } else {

            double minSum = 0;
            set<unsigned int>::iterator bestNonMedoid;

            bool firstAttempt = true;
            for (auto it = nonMedoidsIndexes.begin(); it != nonMedoidsIndexes.end(); it++) {

                // Counting sum of mins
                double sum = 0;
                for (unsigned int j = 0; j < n; j++){
                    sum += std::min(previousMIN[j], distanceMatrix[(*it)*n + j]);
                }

                if (firstAttempt or minSum > sum) {
                    minSum = sum;
                    bestNonMedoid = it;
                    firstAttempt = false;
                }
            }

            targetFunctionValue = minSum;
            unsigned int bestNM = *bestNonMedoid;
            medoidsIndexes.insert(bestNM);
            nonMedoidsIndexes.erase(bestNonMedoid);
            for (unsigned int i = 0; i < n; i++){
                previousMIN[i] = std::min(previousMIN[i], distanceMatrix[bestNM*n + i]);
            }
        }

        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank == 0){
            Dump();
        }
    }

    delete [] previousMIN;
}

// ==================================================================================================================================================
//                                                                                                                            PAM::BuildPhaseParallel
// ==================================================================================================================================================
void PAM::BuildPhaseParallel (const ProcParams& procParams){

    bool debug = false;

    if (not CheckParametersCorrectness())
        return;

    // computation of task's positions for current process from calculation graph

    MPIMessageBUILD localObjFnMin;
    double* previousMIN = new double [n];
    
    for (unsigned int buildIterationsCounter = 0; buildIterationsCounter < k; buildIterationsCounter++) {

        unsigned int tasksNum = nonMedoidsIndexes.size();
        unsigned int tasksPerProcess = std::ceil(static_cast<double>(tasksNum) / procParams.size);
        unsigned int absolutePosition = procParams.rank * tasksPerProcess;

        if (absolutePosition < tasksNum) {

            set<unsigned int>::iterator bestNonMedoid = nonMedoidsIndexes.end();
            auto it = nonMedoidsIndexes.begin();
            for (unsigned int i = 0; i < absolutePosition; i++)
                it++;
            bool firstAttempt = true;

            for (unsigned int i = 0; i < tasksPerProcess and it != nonMedoidsIndexes.end(); i++, it++){

                // Counting sum of mins
                double sum = 0;
                if (medoidsIndexes.size() == 0){
                    for (unsigned int j = 0; j < n; j++){
                        sum += distanceMatrix[(*it)*n + j];
                    }
                } else {
                    for (unsigned int j = 0; j < n; j++){
                        sum += std::min(previousMIN[j], distanceMatrix[(*it)*n + j]);
                    }
                }

                if (firstAttempt or localObjFnMin.objFn > sum) {
                    localObjFnMin.objFn = sum;
                    localObjFnMin.objFn_o_h = *it;
                    firstAttempt = false;
                }
            }

            if (procParams.rank != 0) {

                if (debug) fstream ("output.txt", fstream::out | fstream::app) << "send " << procParams.rank << "->" << 0 << " message = " << localObjFnMin.objFn << " " << localObjFnMin.objFn_o_h << " BUILD_AgregationStep" << endl;

                // Send minSum
                int ret = MPI_Ssend(
                    &localObjFnMin,                         // void* buffer
                    sizeof(MPIMessageBUILD),                // int count
                    MPI_BYTE,                               // MPI_Datatype datatype
                    0,                                      // int dest
                    MPIMessageTypes::BUILD_AgregationStep,  // int tag
                    procParams.comm                         // MPI_Comm comm
                );

                if (ret != MPI_SUCCESS){
                    cout << "Fatal error. '" << procParams.rank << "' failed to send data 'MPIMessageBUILD' to '0' on BUILD_AgregationStep step. Continuing to process data, however program will probably hang." << endl;
                }

            } else {
                // I do not care about order of incoming messages from other processes, but I know how many there is messages to be received
                for (unsigned int i = 1; i < std::ceil(static_cast<double>(tasksNum) / tasksPerProcess); i++){

                    MPI_Status status;
                    MPIMessageBUILD incomingObjFnMin;

                    int ret = MPI_Recv(
                        &incomingObjFnMin,                      // void *buf
                        sizeof(MPIMessageBUILD),                // int count
                        MPI_BYTE,                               // MPI_Datatype datatype
                        MPI_ANY_SOURCE,                         // int source
                        MPIMessageTypes::BUILD_AgregationStep,  // int tag
                        procParams.comm,                        // MPI_Comm comm
                        &status                                 // MPI_Status *status
                    );

                    if (debug) fstream ("output.txt", fstream::out | fstream::app) << "recv " << procParams.rank << "<-" << status.MPI_SOURCE << " message = " << incomingObjFnMin.objFn << " " << incomingObjFnMin.objFn_o_h << " BUILD_AgregationStep" << endl;
                    
                    if (ret != MPI_SUCCESS){
                        cout << "Error. Some problems in receiving 'MPIMessageBUILD' on step 'BUILD_AgregationStep', skipping error and continuing work.";
                    } else {
                        if (localObjFnMin.objFn > incomingObjFnMin.objFn){
                            localObjFnMin.objFn = incomingObjFnMin.objFn;
                            localObjFnMin.objFn_o_h = incomingObjFnMin.objFn_o_h;
                        }
                    }
                }
            }
        }

        // Broadcasting localObjFnMin from process of rank = 0
        MPI_Bcast(
            &localObjFnMin,             // void* buffer
            sizeof(MPIMessageBUILD),    // int count
            MPI_BYTE,                   // MPI_Datatype datatype
            0,                          // int root
            procParams.comm             // MPI_Comm comm
        );


        if (medoidsIndexes.size() == 0){
            for (unsigned int i = 0; i < n; i++){
                previousMIN[i] = distanceMatrix[localObjFnMin.objFn_o_h * n + i];
            }
        } else {
            for (unsigned int i = 0; i < n; i++){
                previousMIN[i] = std::min(previousMIN[i], distanceMatrix[localObjFnMin.objFn_o_h * n + i]);
            }
        }
        targetFunctionValue = localObjFnMin.objFn;
        medoidsIndexes.insert(localObjFnMin.objFn_o_h);
        nonMedoidsIndexes.erase(localObjFnMin.objFn_o_h);


        if (debug) sleep(1);

        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank == 0){
            Dump();
        }

    }

    delete [] previousMIN;
}

// ==================================================================================================================================================
//                                                                                                                             PAM::SwapPhaseParallel
// ==================================================================================================================================================
void PAM::SwapPhaseParallel (const ProcParams& procParams, const unsigned int itMax){

    bool debug = false;

    if (debug){
        sleep(1);
        fstream("output.txt", fstream::out | fstream::app) << "SwapPhase" << endl;
    }

    if (medoidsIndexes.size() >= n or medoidsIndexes.size() <= 1 or not CheckParametersCorrectness())
        return;

    // computation of task's positions for current process from calculation graph
    
    unsigned int tasksNum = k*(n-k);
    unsigned int tasksPerProcess = std::ceil(static_cast<double>(tasksNum) / procParams.size);
    unsigned int absolutePosition = procParams.rank * tasksPerProcess;
    unsigned int task_m_s = absolutePosition / (n-k);
    unsigned int task_o_h = absolutePosition % (n-k);

    if (procParams.rank * tasksPerProcess >= tasksNum) {
        // Somebody given to us more processes then tasks, I am going to use not all processes
        return;
    }

    double* min_m_s = new double [n];

    while (true){

        if (debug) sleep(1);

        // Calculation of best minSum and arguments m_s and o_h

        MPIMessageSWAP localObjFnMin;

        bool firstTimeCalculation = true;
        auto m_s = medoidsIndexes.begin();
        for (unsigned int i = 0; i < task_m_s; i++)
            m_s++;
        bool m_s_changed = true;
        auto o_h = nonMedoidsIndexes.begin();
        for (unsigned int i = 0; i < task_o_h; i++)
            o_h++;

        for (unsigned int computedTasks = 0; computedTasks < tasksPerProcess; computedTasks++){

            if (m_s_changed == true)
            {
                // Computing independent from summ and from o_h changing minimum
                //      (computing min_m_s)
                for (unsigned int i = 0; i < n; i++){
                    auto it = medoidsIndexes.begin();
                    if (*it == *m_s) it++;
                    min_m_s[i] = distanceMatrix[(*it)*n + i]; it++;
                    for (; it != medoidsIndexes.end(); it++){
                        if (*it != *m_s){
                            min_m_s[i] = std::min(min_m_s[i], distanceMatrix[(*it)*n + i]);
                        }
                    }
                }
                m_s_changed = false;
            }

            double sum = 0;
            // Computing sum(min(a,b))
            for (unsigned int i = 0; i < n; i++){
                sum += std::min(min_m_s[i], distanceMatrix[(*o_h)*n + i]);
            }
            if (sum < localObjFnMin.objFn or firstTimeCalculation){
                localObjFnMin.objFn = sum;
                localObjFnMin.objFn_m_s = *m_s;
                localObjFnMin.objFn_o_h = *o_h;
                firstTimeCalculation = false;
            }

            o_h++;
            
            if (o_h == nonMedoidsIndexes.end()){
                o_h = nonMedoidsIndexes.begin();
                m_s++;
                m_s_changed = true;
            }
            if (m_s == medoidsIndexes.end()){
                // Last process could have some shortage of counting tasks
                break;
            }
        }

        int agregate_proc_amount = std::min(procParams.size, static_cast<int>(k));
        // Usually k is far less then number of processes, but for debug purposes, when k can be bigger (and in that case process in their first aggregation step in fact will not aggregate anything)

        if (procParams.rank >= agregate_proc_amount)
        {
            if (debug) fstream ("output.txt", fstream::out | fstream::app) << "send " << procParams.rank << "->" << procParams.rank % agregate_proc_amount << " message = " << localObjFnMin.objFn << " " << localObjFnMin.objFn_m_s << " " << localObjFnMin.objFn_o_h << " SWAP_FirstStepAgregation" << endl;

            // Send minSum
            int ret = MPI_Ssend(
                &localObjFnMin,                             // void* buffer
                sizeof(MPIMessageSWAP),                     // int count
                MPI_BYTE,                                   // MPI_Datatype datatype
                procParams.rank % agregate_proc_amount,     // int dest
                MPIMessageTypes::SWAP_FirstStepAgregation,  // int tag
                procParams.comm                             // MPI_Comm comm
            );

            if (ret != MPI_SUCCESS){
                cout << "Fatal error. '" << procParams.rank << "' failed to send data 'MPIMessageSWAP' to '" << procParams.rank % agregate_proc_amount << "' on SWAP_FirstStepAgregation step. Continuing to process data, however program will probably hang." << endl;
            }

        } else {
            // Getting sended minimums, by other processes and counting minimum
            // for (int i = agregate_proc_amount + procParams.rank; i < procParams.size; i+= agregate_proc_amount) {
            for (int i = agregate_proc_amount + procParams.rank; i < std::ceil(static_cast<float>(tasksNum) / tasksPerProcess); i+= agregate_proc_amount) {
            // for (int i = agregate_proc_amount + procParams.rank; i < std::min(procParams.size, static_cast<int>(tasksNum)); i+= agregate_proc_amount) {

                MPI_Status status;
                MPIMessageSWAP incomingObjFnMin;

                // I do not care about order of incoming messages from other processes, but I know how many there is messages to be received
                int ret = MPI_Recv(
                    &incomingObjFnMin,                          // void *buf
                    sizeof(MPIMessageSWAP),                     // int count
                    MPI_BYTE,                                   // MPI_Datatype datatype
                    MPI_ANY_SOURCE,                             // int source
                    MPIMessageTypes::SWAP_FirstStepAgregation,  // int tag
                    procParams.comm,                            // MPI_Comm comm
                    &status                                     // MPI_Status *status
                );

                if (debug) fstream ("output.txt", fstream::out | fstream::app) << "recv " << procParams.rank << "<-" << status.MPI_TAG << " message = " << incomingObjFnMin.objFn << " " << incomingObjFnMin.objFn_m_s << " " << incomingObjFnMin.objFn_o_h << " SWAP_FirstStepAgregation" << endl;
                if (ret != MPI_SUCCESS){
                    cout << "Error. Some problems in receiving 'MPIMessageSWAP' on step 'SWAP_FirstStepAgregation', skipping error and continuing work.";
                } else {
                    if (localObjFnMin.objFn > incomingObjFnMin.objFn){
                        localObjFnMin.objFn = incomingObjFnMin.objFn;
                        localObjFnMin.objFn_m_s = incomingObjFnMin.objFn_m_s;
                        localObjFnMin.objFn_o_h = incomingObjFnMin.objFn_o_h;
                    }
                }
            }

            // Agregating ObjFnMin on process with rank = 0

            if (procParams.rank != 0){

                if (debug) fstream ("output.txt", fstream::out | fstream::app) << "send " << procParams.rank << "->" << 0 << " message = " << localObjFnMin.objFn << " " << localObjFnMin.objFn_m_s << " " << localObjFnMin.objFn_o_h << " SWAP_SecondStepAgregation" << endl;

                // Send minSum
                int ret = MPI_Ssend(
                    &localObjFnMin,                         // void* buffer
                    sizeof(MPIMessageSWAP),                     // int count
                    MPI_BYTE,                               // MPI_Datatype datatype
                    0,                                      // int dest
                    MPIMessageTypes::SWAP_SecondStepAgregation,  // int tag
                    procParams.comm                         // MPI_Comm comm
                );

                if (ret != MPI_SUCCESS){
                    cout << "Fatal error. '" << procParams.rank << "' failed to send data 'MPIMessageSWAP' to '0' on SWAP_SecondStepAgregation step. Continuing to process data, however program will probably hang." << endl;
                }

            } else {
                for (int i = 1; i < agregate_proc_amount; i++){

                    MPI_Status status;
                    MPIMessageSWAP incomingObjFnMin;

                    // I do not care about order of incoming messages from other processes, but I know how many there is messages to be received
                    int ret = MPI_Recv(
                        &incomingObjFnMin,                      // void *buf
                        sizeof(MPIMessageSWAP),                     // int count
                        MPI_BYTE,                               // MPI_Datatype datatype
                        MPI_ANY_SOURCE,                         // int source
                        MPIMessageTypes::SWAP_SecondStepAgregation,  // int tag
                        procParams.comm,                        // MPI_Comm comm
                        &status                                 // MPI_Status *status
                    );

                    if (debug) fstream ("output.txt", fstream::out | fstream::app) << "recv " << procParams.rank << "<-" << status.MPI_SOURCE << " message = " << incomingObjFnMin.objFn << " " << incomingObjFnMin.objFn_m_s << " " << incomingObjFnMin.objFn_o_h << " SWAP_SecondStepAgregation" << endl;
                    if (ret != MPI_SUCCESS){
                        cout << "Error. Some problems in receiving 'MPIMessageSWAP' on step 'SWAP_SecondStepAgregation', skipping error and continuing work.";
                    } else {
                        if (localObjFnMin.objFn > incomingObjFnMin.objFn){
                            localObjFnMin.objFn = incomingObjFnMin.objFn;
                            localObjFnMin.objFn_m_s = incomingObjFnMin.objFn_m_s;
                            localObjFnMin.objFn_o_h = incomingObjFnMin.objFn_o_h;
                        }
                    }
                }
            }
        }

        // Broadcasting localObjFnMin from process of rank = 0
        MPI_Bcast(
            &localObjFnMin,     // void* buffer
            sizeof(MPIMessageSWAP), // int count
            MPI_BYTE,           // MPI_Datatype datatype
            0,                  // int root
            procParams.comm     // MPI_Comm comm
        );

        if (targetFunctionValue <= localObjFnMin.objFn){
            break;

        } else {

            // Patching medoidsIndexes multitude in all processes
            medoidsIndexes.erase(localObjFnMin.objFn_m_s);
            nonMedoidsIndexes.insert(localObjFnMin.objFn_m_s);
            medoidsIndexes.insert(localObjFnMin.objFn_o_h);
            nonMedoidsIndexes.erase(localObjFnMin.objFn_o_h);
            targetFunctionValue = localObjFnMin.objFn;
        }

        if (debug) sleep(1);

        int rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        if (rank == 0)
            Dump();

        iterationsCounter ++;
        if (itMax != 0 && iterationsCounter < itMax ){
            break;
        }
    }

    delete [] min_m_s;
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

    fout << "nonMedoidsIndexes:" << endl;
    for (auto it = nonMedoidsIndexes.begin(); it != nonMedoidsIndexes.end(); it++){
        fout << *it << " ";
    }
    fout << endl;

    fout << "distanceMatrix:" << endl << std::fixed;
    for (unsigned int i = 0; i < n; i++){
        for (unsigned int j = 0; j < n; j++){
            fout << std::setprecision(5) << distanceMatrix[i*n + j] << " ";
        }
        fout << endl;
    }

    fout << "========== Dump end ==========" << endl;
    fout.close();
}

// ==================================================================================================================================================
//                                                                                                                     AM::CheckParametersCorrectness
// ==================================================================================================================================================
bool PAM::CheckParametersCorrectness () const{
    if (k <= 0 or k > n or medoidsIndexes.size() + nonMedoidsIndexes.size() != n){
        return false;
    } else {
        return true;
    }
}
