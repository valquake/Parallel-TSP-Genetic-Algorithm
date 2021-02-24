#include <ff/ff.hpp>
#include <ff/farm.hpp>

#include "population.hpp"
#include "../utils/Random.hpp"

using namespace ff;
using namespace std;

struct Task {
    Task(std::unique_ptr<Population> pop):subP(std::move(pop)){};

 	std::unique_ptr<Population> subP;
 	std::unique_ptr<Population> result;
};

struct Master: ff_node_t<Task> {
    Master(const int nw, const size_t pops, const size_t n, const size_t num_it, std::shared_ptr<SymmMatrix> m, const int seed)
    :nw(nw), maxIt(num_it){
    	population.reset(new Population(pops, n, std::move(m), seed));
    	nPopulation = population->getSize();
    }
  
    // -------------------

    Task *svc(Task *t) {
    	if(t == nullptr){	// Start: no task submitted yet, begin with sum phase
			std::vector<Population> v;

			#ifdef TIME
				auto start = std::chrono::system_clock::now();
			#endif

			// Compute initial chunks of population to submit to threadpool
			v = population->init_chunks_of_population(nw);

			#ifdef TIME
				auto stop = std::chrono::system_clock::now();
			    std::chrono::duration<double> elapsed = stop - start;
			    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

			    tAssignChunk += musec;
			#endif

			// submit tasks to workers (for the first time)
			for(int i = 0; i < v.size(); i++){
				Population tmp = std::move(v[i]);
				ff_send_out(new Task(std::make_unique<Population>(tmp)));
			}

			// waiting for completing nw tasks
			tasks = nw;
			numIt++;
		}else{

			if(--tasks == 0){ // All threads have send back their result
				partials.push_back(*(t->result));

				delete t;

				if(numIt != maxIt){ // We are not at the end
					{
						#ifdef TIME
							auto start = std::chrono::system_clock::now();
						#endif

						// include new generations in population
						population->updatePopulation(partials);

						#ifdef TIME
							auto stop = std::chrono::system_clock::now();
						    std::chrono::duration<double> elapsed = stop - start;
						    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
			
						    tMerge += musec;
						#endif
					}

					std::vector<std::vector<Chromosome>> pl;

					{
						#ifdef TIME
							auto start = std::chrono::system_clock::now();
						#endif

						// compute new chunks
						pl = population->get_chunks_of_population(nw);

						#ifdef TIME
							auto stop = std::chrono::system_clock::now();
						    std::chrono::duration<double> elapsed = stop - start;
						    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();

						    tAssignChunk += musec;
						#endif
					}

					// send out chunks of population
					for (int i = 0; i < pl.size(); ++i){
						partials[i].setPopulation(pl[i]);
						Population tmp = std::move(partials[i]);
						ff_send_out(new Task(std::make_unique<Population>(tmp)));
					}

					tasks = nw;
					partials.clear();
					numIt++;

				} else { // End of algorithm
					#ifdef TIME
						auto start = std::chrono::system_clock::now();
					#endif

					// include (for the last time) new generation
					population->updatePopulation(partials);

					#ifdef TIME
						auto stop = std::chrono::system_clock::now();
					    std::chrono::duration<double> elapsed = stop - start;
					    auto musec = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		
					    tMerge += musec;
					#endif

					return EOS;
				}
			} else { // not all tasks have been completed
				partials.push_back(*(t->result));

				delete t;
			}
		}

        return GO_ON;
    }

    void svc_end() {
    	// At the end, print overall result
    	#ifdef TIME 
		    std::cout << "	 TOTAL time for assigning chunks: " << tAssignChunk  << " usec " << std::endl;
			std::cout << "	 TOTAL time for merging: " << tMerge << " usec " << std::endl;	    
			std::cout << std::endl;
		#endif

		#ifdef VERBOSE
    		population->printResults();
    	#endif
    }
    
    int totalSum = 0, nPopulation = 0;
    int tasks = 0, numIt = 0;
    std::unique_ptr<Population> population;
    std::vector<Population> partials;
    const int nw, maxIt;
    int tAssignChunk = 0, tMerge = 0;
};

struct Slave: ff_node_t<Task> {
	Slave(const int prob1, const int prob2, std::mutex &mut):prob_mutation(prob1),prob_crossover(prob2), m(mut){};


    Task *svc(Task *t) {
    	auto p = *(t->subP);
		int nP = p.getSize(), idx = 0;
		int tBreed=0, tSel=0, tNewGen=0, tCros=0, tMut=0;

		int num_crossover = (nP * prob_crossover) / 100;
		int num_mutation = (nP * prob_mutation) / 100;

		{
			#ifdef TIMETHREAD 
				utimer t("Total time algo (for thread)");
			#endif

			{
				#ifdef TIMETHREAD 
					auto start = std::chrono::system_clock::now();
				#endif

				// utimer t("Breeding");

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
						//utimer t("Mutuation");
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

		Population s = std::move(p);

		// Reuse the same task to commit results... set pointer
		t->result = std::make_unique<Population>(s);
		ff_send_out(t);
		return GO_ON;
    }

    const int prob_mutation;
    const int prob_crossover;
    std::mutex &m;
};
