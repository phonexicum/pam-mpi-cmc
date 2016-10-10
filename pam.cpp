#include <mpi.h>

#include <fstream>

#include <unistd.h>

using std::fstream;
using std::endl;

int main(int argc, char* argv[])
{
    int rank;
    int size;
    char hostname[256];

    MPI_Init(&argc,&argv); // Start MPI

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);   // get current process id
    MPI_Comm_size (MPI_COMM_WORLD, &size);  // get number of processes


    gethostname(hostname, 255);

    fstream fout ("output.txt", fstream::out | fstream::app);
    fout << "Hello world!  I am process number: " << rank << " on host " << hostname << endl;
    fout.close();


    MPI_Finalize(); // Stop MPI
    return 0;
}
