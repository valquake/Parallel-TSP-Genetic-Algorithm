#include <iostream>
#include <thread>
#include <memory>
#include <vector>

#include <stdio.h>
#include <sys/stat.h>
#include <fstream>
#include <chrono>

#include "../include/symm.hpp"
#include "../include/chromosome.hpp"
#include "../include/GA_seq.hpp"
#include "../include/population.hpp"

using namespace std;

static inline void usage(const char *argv0) {
    printf("--------------------\n");
    printf("Usage: %s <seed> <n-population> <n-iteration> <prob-mutation> <prob-crossover> <filename>\n", argv0);
    printf("--------------------\n");
}

static inline void usage_nofile(const char *argv0) {
    printf("--------------------\n");
    printf("Usage: %s <seed> <n-population> <n-iteration> <prob-mutation> <prob-crossover> <fn-nodes> <max-weigth>\n", argv0);
    printf("--------------------\n");
}

void adjancency_matrix(const char **argv, int argc, SymmMatrix &m){
	if(argc == 8){
	    const size_t nodes 		= stol(argv[6]);
	    const size_t maxWeight 	= stol(argv[7]);

	    #ifdef TIME
			auto start = std::chrono::system_clock::now();
		#endif

		m.setNodes(nodes);
		m.input_random(maxWeight);

	    #ifdef TIME
		    auto stop = std::chrono::system_clock::now();
		    std::chrono::duration<double> elapsed = stop - start;
		    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	    
	    	std::cout << "Matrix distances building (randomnly) " << " computed in " << musec << " usec " << std::endl;
	    #endif

	    return;
    }

    usage_nofile(argv[0]);
    exit(-1);
}

void adjacency_matrix_from_filename(const char **argv, int argc, SymmMatrix &m){
	if(argc == 7){
		const char* fname = argv[6];
	    struct stat sb;

	    // Check if file exists
	    if (stat(fname, &sb) == 0) {
	    	#ifdef TIME
	    		auto start = std::chrono::system_clock::now();
	    	#endif

	        std::ifstream infile(fname);
	        int n, e1, e2, w;

	        // In the first line there's number of nodes
	        infile >> n;
	        m.setNodes(n);

			while (infile >> e1 >> e2 >> w) {
				if(e1 != e2)
					m.insert(e1, e2, w);
			}

			#ifdef TIME
			    auto stop = std::chrono::system_clock::now();
			    std::chrono::duration<double> elapsed = stop - start;
			    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		    
		    	std::cout << "Matrix distances building (from file) " << " computed in " << musec << " usec " << std::endl;
		    #endif

			return;
	    }

	    std::cout << "File not found!" << std::endl;

	}

	usage(argv[0]);
	exit(-1);
}

int main(int argc, char const *argv[])
{
	if (argc < 6) {
        #ifdef FILEMATRIX
    		usage(argv[0]);
    	#else
    		usage_nofile(argv[0]);
    	#endif
    		
        return -1;
    }

    const size_t seed 	= stol(argv[1]);
    const size_t np 	= stol(argv[2]);
    const size_t it 	= stol(argv[3]);
    const size_t p_m 	= stol(argv[4]);
    const size_t p_c 	= stol(argv[5]);

    if(p_c > 100 || p_c < 1 || p_m > 100 || p_m < 1){
    	std::cout << "Probabilities must be integer number between 1 and 100!" << std::endl;
    	
    	#ifdef FILEMATRIX
    		usage(argv[0]);
    	#else
    		usage_nofile(argv[0]);
    	#endif

        return -1;
    }

    srand(seed);
    SymmMatrix distances;

    #ifdef FILEMATRIX
    	adjacency_matrix_from_filename(const_cast<const char**>(&argv[0]), argc, distances);
    #else
    	adjancency_matrix(const_cast<const char**>(&argv[0]), argc, distances);
    #endif

    auto start = std::chrono::system_clock::now();

    GeneticAlgorithmTSP ga = GeneticAlgorithmTSP(np, distances.getNodes(), make_shared<SymmMatrix>(distances), p_m, p_c, it, seed);
    ga.runAlgorithm();

    auto stop = std::chrono::system_clock::now();
  	auto elapsed = stop-start;
  	auto musec = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();

  	double tot = (double) musec / 1000.0;

  	#ifdef VERBOSE 
  		std::cout << "Tot SEQ TIME: ";
  	#endif
  		
  		std::cout << tot << std::endl;

	return 0;
}