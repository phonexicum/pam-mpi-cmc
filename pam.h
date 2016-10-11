#include <set>
#include <string>

using std::set;
using std::string;

#include <mpi.h>

// ==================================================================================================================================================
//                                                                                                                                         ProcParams
// ==================================================================================================================================================
struct ProcParams {
    int rank;
    int size;

        public:
    
    ProcParams(){
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // get current process id
        MPI_Comm_size (MPI_COMM_WORLD, &size);  // get number of processes
    }
};

// ==================================================================================================================================================
//                                                                                                                                                PAM 
// ==================================================================================================================================================
class PAM {

    const int n; // Multitude power
    const int k; // Clusters number
    const double* distanceMatrix;

    set<int> medoidsIndexes;
    int iterationsCounter;
    double targetFunctionValue;

    ProcParams procParams;

        public:
    // PAM will take ownership on distanceMatrix_ and delete it in its destructor
    PAM(const int n_, const int k_, double* distanceMatrix_);
    ~PAM();

    // FUTURE WORK: BuildPhase calculation can be parallelized
    void BuildPhase ();
    // SwapPhase is parallelized
    // itMax - maximum number of iterations
    void SwapPhase (const ProcParams& procParams, const int itMax);

    void Dump (const string& fout_str) const;

    double getTargetFunctionValue() { return targetFunctionValue; }
    set<int> getMedoids ()          { return medoidsIndexes; }
    int getK()                      { return k; }
    int getIterationsCounter()      { return iterationsCounter; }

};
