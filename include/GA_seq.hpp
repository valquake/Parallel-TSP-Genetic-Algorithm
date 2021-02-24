#ifndef GA_H
#define GA_H

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>

#include "symm.hpp"
#include "chromosome.hpp"
#include "population.hpp"
#include "../utils/utimer.cpp"

typedef std::mt19937 rng_type;

class GeneticAlgorithmTSP
{

private:
	std::unique_ptr<Population> population;
	int nPopulation, maxIterations;
	int prob_mutuation = 50, prob_crossover = 50, seed = 0;
	long tAlgo = 0.0;

	int tBreed = 0, tSel = 0, tNewGen = 0, tCros = 0, tMut = 0;
	// Take into account the BEST fitness value population
	int best_fitness = __INT_MAX__;

public:
	
	GeneticAlgorithmTSP(const size_t nPopulation, const size_t n, std::shared_ptr<SymmMatrix> m, const size_t prob1, const size_t prob2, const size_t max, const size_t seed)
	: nPopulation(nPopulation), prob_mutuation(prob1), prob_crossover(prob2), maxIterations(max), seed(seed){

		population.reset(new Population(nPopulation, n, std::move(m), seed));
	};

	void runAlgorithm(){
		bool TrueFalse = false;

		int totIt = 0;
		int idx = 0;

		nPopulation = population->getSize();

		// Compute in advance for how many individuals we have to do mutation and crossover
		int num_crossover = (nPopulation * prob_crossover) / 100;
		int num_mutation = (nPopulation * prob_mutuation) / 100;

		#ifdef TIME 
			auto startA = std::chrono::system_clock::now();
		#endif

		for(int j = 0; j < maxIterations ; ++j){

			{
				#ifdef TIME 
					auto start = std::chrono::system_clock::now();
				#endif

				// utimer t("Breeding");

				// We proceed by applying transformations for a "num_mutation" elements
				for (int i = 0; i < num_mutation; ++i)
				{
					{	
						#ifdef TIME
							auto start = std::chrono::system_clock::now();
						#endif

						//utimer t("Selection for Mutuation&Crossover");
						idx = population->roulette_wheel_selection();

						#ifdef TIME 
							auto stop = std::chrono::system_clock::now();
						    std::chrono::duration<double> elapsed = stop - start;
						    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

						    tSel += musec;
						#endif
					}
					
					{
						//utimer t("Mutuation");
						#ifdef TIME 
							auto start = std::chrono::system_clock::now();
						#endif

						population->doMutation(idx);

						#ifdef TIME 
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
						//utimer t("Selection for Mutuation&Crossover");
						#ifdef TIME
							auto start = std::chrono::system_clock::now();
						#endif

						while(idx_chr.first == idx_chr.second){
							idx_chr.first = population->roulette_wheel_selection();
							idx_chr.second = population->roulette_wheel_selection();
						}

						if(idx_chr.first > idx_chr.second)
							std::swap(idx_chr.first, idx_chr.second);

						#ifdef TIME 
							auto stop = std::chrono::system_clock::now();
						    std::chrono::duration<double> elapsed = stop - start;
						    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

						    tSel += musec;
						#endif
					}

					{
						//utimer t("Crossover");
						#ifdef TIME 
							auto start = std::chrono::system_clock::now();
						#endif

						population->doCrossover(idx_chr.first, idx_chr.second);

						#ifdef TIME 
							auto stop = std::chrono::system_clock::now();
						    std::chrono::duration<double> elapsed = stop - start;
						    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

						    tCros += musec;
					    #endif	
				    }										
				}

				#ifdef TIME
					auto stop = std::chrono::system_clock::now();
				    std::chrono::duration<double> elapsed = stop - start;
				    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

				    tBreed += musec;
				#endif
			}

			{	
				#ifdef TIME
					auto start = std::chrono::system_clock::now();
				#endif

				//utimer t("New Generation");
				population->AdjustPopulation(nPopulation);


				#ifdef TIME
					auto stop = std::chrono::system_clock::now();
				    std::chrono::duration<double> elapsed = stop - start;
				    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

				    tNewGen += musec;
				#endif
			}

			totIt++;
		}


		#ifdef TIME 
			auto stopA = std::chrono::system_clock::now();
		    std::chrono::duration<double> elapsedA = stopA - startA;
		    tAlgo = std::chrono::duration_cast<std::chrono::microseconds>(elapsedA).count();

			std::cout << " 	 Time for Selection: " << tSel << " usec " << std::endl;
			std::cout << " 	 Time for Breeding: " << tBreed << " usec " << std::endl;
			std::cout << " 	 Total for Mutuation: " << tMut << std::endl;
			std::cout << " 	 Total for Crossover: " << tCros << std::endl;
			std::cout << " 	 Time for New Generation: " << tNewGen << " usec " << std::endl;
			std::cout << "=> TOTAL time algorithm: " << tAlgo << " usec " << std::endl;
			std::cout << std::endl;
		#endif

		#ifdef VERBOSE
			population->printResults();	
		#endif	
	}
	
};


#endif