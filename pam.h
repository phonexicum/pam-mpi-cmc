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
    MPI_Comm comm;

        public:
    
    ProcParams(MPI_Comm mpiComm = MPI_COMM_WORLD){
        comm = mpiComm;
        MPI_Comm_rank (comm, &rank); // get current process id
        MPI_Comm_size (comm, &size); // get number of processes
    }
};

// ==================================================================================================================================================
//                                                                                                                                                PAM
// ==================================================================================================================================================
class PAM {

        public:

    PAM(const unsigned int n_, const unsigned int k_, const double* distanceMatrix_);
    ~PAM();

    void BuildPhaseConsecutive ();
    void BuildPhaseParallel (const ProcParams& procParams);

    // SwapPhase is parallelized
    // 
    // ASSUMPTION: state of PAM data structure must be equal for all processes before SwapPhase and will be equal after SwapPhase
    // 
    // itMax - maximum number of iterations
    // 
    void SwapPhaseParallel (const ProcParams& procParams, const unsigned int itMax);

    void Dump (const string& fout_str = string("output.txt")) const;

    double getTargetFunctionValue() { return targetFunctionValue; }
    set<unsigned int> getMedoids ()          { return medoidsIndexes; }
    unsigned int getK()                      { return k; }
    unsigned int getIterationsCounter()      { return iterationsCounter; }

        private:

    static const bool debug = false;

    const unsigned int n; // Multitude power
    const unsigned int k; // Clusters number
    const double* distanceMatrix;

    set<unsigned int> medoidsIndexes;
    set<unsigned int> nonMedoidsIndexes;
    unsigned int iterationsCounter;
    double targetFunctionValue;

    ProcParams procParams;

    bool CheckParametersCorrectness() const;

        private:

    struct MPIMessageBUILD {
        double objFn;
        unsigned int objFn_o_h;

        MPIMessageBUILD(): objFn(0), objFn_o_h(0) {}
    };

    struct MPIMessageSWAP {
        double objFn;
        unsigned int objFn_m_s;
        unsigned int objFn_o_h;

        MPIMessageSWAP(): objFn(0), objFn_m_s(0), objFn_o_h(0) {}
    };

    enum MPIMessageTypes {
        SWAP_FirstStepAgregation,
        SWAP_SecondStepAgregation,
        SWAP_BroadCastNewModifications,
        BUILD_AgregationStep
    };

};
