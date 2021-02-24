#ifndef GAP_H
#define GAP_H

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <queue>

#include "symm.hpp"
#include "chromosome.hpp"
#include "population.hpp"
#include "../utils/utimer.cpp"
#include "ThreadpoolTask.hpp"

#define EOS nullptr

typedef std::vector<Population> subpopulations;
typedef std::shared_ptr<std::packaged_task<Population()>> shared_task;

class ParallelGeneticAlgorithmTSP
{

private:
	subpopulations 												v;
	std::shared_ptr<SymmMatrix> 								distances;
	std::unique_ptr<ThreadpoolTask<Population>> 	tpl;
	int 														nodes = 0, nPopulation = 0, maxIterations = 0, nWorkers = 0, seed = 0;
	int 														prob_mutuation = 50, prob_crossover = 50, tAlgo = 0;

	Population runAlgorithm_subP(Population &p){
		int tBreed=0, tSel=0, tNewGen=0, tCros=0, tMut=0;
		int nP = p.getSize(), idx = 0;

		int num_crossover = (nP * prob_crossover) / 100;
		int num_mutation = (nP * prob_mutuation) / 100;

		{
			#ifdef TIMETHREAD 
				utimer t("Total time algo (for thread)");
			#endif

			{
				#ifdef TIMETHREAD 
					auto start = std::chrono::system_clock::now();
				#endif

				// utimer t("Breeding");

				// We proceed by applying transformations for [idx_chr, size - 2] elements
				for (int i = 0; i < num_mutation; ++i)
				{
					{	
						#ifdef TIMETHREAD
							auto start = std::chrono::system_clock::now();
						#endif

						//utimer t("Selection for Mutuation&Crossover");
						idx = p.roulette_wheel_selection();

						#ifdef TIMETHREAD 
							auto stop = std::chrono::system_clock::now();
						    std::chrono::duration<double> elapsed = stop - start;
						    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

						    tSel += musec;
						#endif
					}
					
					{
						#ifdef TIMETHREAD 
							auto start = std::chrono::system_clock::now();
						#endif

						p.doMutation(idx);

						#ifdef TIMETHRAD 
							auto stop = std::chrono::system_clock::now();
						    std::chrono::duration<double> elapsed = stop - start;
						    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

						    tMut += musec;
					    #endif
					}						
				}

				for(int i = 0; i < num_crossover; i++)
				{
					std::pair<int,int> idx_chr = std::make_pair(0, 0);

					{
						#ifdef TIMETHREAD
							auto start = std::chrono::system_clock::now();
						#endif

						while(idx_chr.first == idx_chr.second){
							idx_chr.first = p.roulette_wheel_selection();
							idx_chr.second = p.roulette_wheel_selection();
						}

						if(idx_chr.first > idx_chr.second)
							std::swap(idx_chr.first, idx_chr.second);

						#ifdef TIMETHREAD 
							auto stop = std::chrono::system_clock::now();
						    std::chrono::duration<double> elapsed = stop - start;
						    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

						    tSel += musec;
						#endif
					}

					{
						#ifdef TIMETHREAD 
							auto start = std::chrono::system_clock::now();
						#endif

						p.doCrossover(idx_chr.first, idx_chr.second);

						#ifdef TIMETHREAD
							auto stop = std::chrono::system_clock::now();
						    std::chrono::duration<double> elapsed = stop - start;
						    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

						    tCros += musec;
					    #endif	
				    }										
				}

				#ifdef TIMETHREAD
					auto stop = std::chrono::system_clock::now();
				    std::chrono::duration<double> elapsed = stop - start;
				    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

				    tBreed += musec;
				#endif
			}

			{	
				#ifdef TIMETHREAD
					auto start = std::chrono::system_clock::now();
				#endif

				// utimer t("Rank&Select");
				p.AdjustPopulation(nP);


				#ifdef TIMETHREAD
					auto stop = std::chrono::system_clock::now();
				    std::chrono::duration<double> elapsed = stop - start;
				    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

				    tNewGen += musec;
				#endif
			}

			

			#ifdef TIMETHREAD 
				std::cout << std::endl;
				std::cout << "Total for Breeding: " << tBreed << std::endl;
				std::cout << "Total for Mutuation: " << tMut << std::endl;
				std::cout << "Total for Crossover: " << tCros << std::endl;
				std::cout << "Total for Selection: " << tSel << std::endl;
				std::cout << "Total for New Generation: " << tNewGen << std::endl;
			#endif
		}

		return p;
	}

	void runParallelAlgorithm(){
		std::deque<std::future<Population>> futures_t;

	    // For each part, calculate size and run algorithm on a separate thread.
	    for (std::size_t j = 0; j != v.size(); ++j) {
	        std::packaged_task<Population()> task(std::bind(&ParallelGeneticAlgorithmTSP::runAlgorithm_subP, this, v[j]));	
	  		shared_task pt = std::make_shared<std::packaged_task<Population()>>(std::packaged_task<Population()>(std::move(task)));	

	  		// Submit the task to threadpool
	        futures_t.emplace_back(tpl->submit(std::ref(pt)));
	    }

	    // Collect partial results
	    int j = 0;
	   	while(!futures_t.empty()) { 
	   		auto f = std::move(futures_t.front());
			futures_t.pop_front();
			v[j] = f.get();
			j++;
		}
	}

public:
	
	ParallelGeneticAlgorithmTSP(const size_t nw, const size_t nPopulation, const size_t n, std::shared_ptr<SymmMatrix> m, const size_t prob1, const size_t prob2, const size_t it, const int seed)
	: nodes(n), nPopulation(nPopulation), prob_mutuation(prob1), prob_crossover(prob2), seed(seed), nWorkers(nw), maxIterations(it), distances(std::move(m)){};

	void runAlgorithm(){
		Population p(nPopulation, nodes, std::move(distances), seed);
		int tAssignChunk = 0, tMerge = 0, tWorker = 0;

		#ifdef TIME
			auto startA = std::chrono::system_clock::now();
		#endif

		// Define threadpool of workers
		tpl.reset(new ThreadpoolTask<Population>(nWorkers));

		{
			#ifdef TIME
				auto start = std::chrono::system_clock::now();
			#endif

			// Compute initial chunks of population to submit to threadpool
			v = p.init_chunks_of_population(nWorkers);

			#ifdef TIME
				auto stop = std::chrono::system_clock::now();
			    std::chrono::duration<double> elapsed = stop - start;
			    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

			    tAssignChunk += musec;
			#endif
		}
		
		for(int j = 0; j < maxIterations ; ++j){

			// If we aren't in the first iteration, update vector of subpopulations
			if (j != 0){
				#ifdef TIME
					auto start = std::chrono::system_clock::now();
				#endif

				auto allV = p.get_chunks_of_population(nWorkers);

				for(int i = 0; i < allV.size(); i++)
					v[i].setPopulation(allV[i]);

				#ifdef TIME
					auto stop = std::chrono::system_clock::now();
				    std::chrono::duration<double> elapsed = stop - start;
				    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

				    tAssignChunk += musec;
				#endif
			}

			{
				#ifdef TIME
					auto start = std::chrono::system_clock::now();
				#endif

				// run GA algorithm (one iteration) in parallel
				runParallelAlgorithm();

				#ifdef TIME
					auto stop = std::chrono::system_clock::now();
				    std::chrono::duration<double> elapsed = stop - start;
				    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

				    tWorker += musec;
				#endif
			}

			{
				#ifdef TIME
					auto start = std::chrono::system_clock::now();
				#endif

				// merge resulting subpopulations
				p.updatePopulation(v);
	
				#ifdef TIME
					auto stop = std::chrono::system_clock::now();
				    std::chrono::duration<double> elapsed = stop - start;
				    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	
				    tMerge += musec;
				#endif
			}
		}

		// shutdown threadpool of workers
		tpl->shutdownAll();


		#ifdef TIME 
			auto stopA = std::chrono::system_clock::now();
		    std::chrono::duration<double> elapsedA = stopA - startA;
		    tAlgo = std::chrono::duration_cast<std::chrono::microseconds>(elapsedA).count();	    

		    std::cout << "	 TOTAL time for assigning chunks: " << tAssignChunk  << " usec " << std::endl;
		    std::cout << "	 TOTAL time for workers: " << tWorker  << " usec " << std::endl;
			std::cout << "	 TOTAL time for merging: " << tMerge << " usec " << std::endl;	    
			std::cout << "=> TOTAL time algorithm: " << tAlgo << " usec " << std::endl;
			std::cout << std::endl;
		#endif

		#ifdef VERBOSE
			p.printResults();
		#endif
	}
	
};


#endif