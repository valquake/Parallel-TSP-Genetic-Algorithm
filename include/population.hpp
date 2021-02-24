#ifndef POPULATION_H
#define POPULATION_H

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <chrono>
#include <future>
#include <execution>

#include "symm.hpp"
#include "chromosome.hpp"


class Population
{
private:
	std::vector<Chromosome> population;
	std::vector<Chromosome> nextGen; // It is necessary a vector apart, storing all new chromosomes generated by mutation and crossover
	std::shared_ptr<SymmMatrix> distances;
	std::shared_ptr<Random> r;
	int nodes = 0, nPopulation = 0, seed = 0;
	int numIt = 0, totIt = 0, maxIteration = 200, sumFitness = 0, initialBestFitness = 0;

	// Take into account the BEST and the WORST fitness values for the population
	int best_fitness = __INT_MAX__, worst_fitness = 0;

	double totalFitness = 0.0;

	bool isEqual(const Chromosome &p){
		for (auto it = population.begin(); it != population.end(); ++it)
		{
			if(*it == p)
				return true;
		}

		return false;
	}


public:
	
	// constructor, given vector of chromosomes
	Population(std::vector<Chromosome> pop, const size_t n, std::shared_ptr<SymmMatrix> m, std::shared_ptr<Random> rand)
	: nodes(n), distances(std::move(m)), r(std::move(rand)), population(std::move(pop)){

		nPopulation = population.size();

		nextGen.reserve(nPopulation);

		totalFitness = std::accumulate(population.begin(), population.end(), 0.0, [](double acc, const Chromosome& a){
			return acc + (double) a.getFitness();
		});
		
		initialBestFitness = population[0].getFitness();
		worst_fitness = population[nPopulation - 1].getFitness();
		best_fitness = population[0].getFitness();
	};

	// standard constructor
	Population(const size_t pops, const size_t n, std::shared_ptr<SymmMatrix> m, const int seed)
	: nodes(n), distances(std::move(m)), seed(seed){

		// Pay attention to the population size, if there are too few nodes.
		if (n < 10) {
			auto fact = utils::factorial(n);
			nPopulation = (pops >= fact)? fact : pops;
		}
		else nPopulation = pops;

		population.reserve(nPopulation);
		r.reset(new Random(nodes, nPopulation, seed));


		for (int i = 0; i < nPopulation; ++i) {
			Chromosome chr(nodes, distances, r);

			if(!isEqual(chr))
				population.push_back(chr);				
		}


		std::sort(population.begin(), 
						population.end());	

		nPopulation = population.size();

		totalFitness = std::accumulate(population.begin(), population.end(), 0.0, [](double acc, const Chromosome& a){
			return acc + (double) a.getFitness();
		});

		nextGen.reserve(nPopulation);

		initialBestFitness = population[0].getFitness();
		worst_fitness = population[nPopulation - 1].getFitness();
		best_fitness = initialBestFitness;
	};

	int const &getBestFitness() const{
		return best_fitness;
	}

	int const &getSize() const{
		return nPopulation;
	}

	void setPopulation(std::vector<Chromosome> const &newV){
		population = newV;
	}

	std::vector<Chromosome> const &getPopulation() const{
		return population;
	}

	std::vector<Chromosome> const &getNextGen() const{
		return nextGen;
	}

	void doMutation(const int i){
		auto chr = population[i].mutation(std::ref(r));
		nextGen.push_back(std::move(chr));
	}

	void doCrossover(const int i, const int j){
		int first = 0, second = 0;

		while(first == second){
			// pick a random element
			first = r->random_node(0);
		  	// pick another random element
		  	second = r->random_node(0);
	  	}

	  	if(first > second)
	  		std::swap(first, second);

	  	if(second == first + 1){
	  		if(second != nodes - 1)
	  			second ++;
	  		else
	  			first --;
	  	}

	  	// Generate son from chrossover transformation (i to j)
		auto chr = population[i].crossover(first, second, std::move(population[j].getChromosome()));
		nextGen.push_back(std::move(chr));		

		// Generate son from chrossover transformation (j to i)
		auto chr2 = population[j].crossover(first, second, std::move(population[i].getChromosome()));
		nextGen.push_back(std::move(chr2));
	}

	void printResults(){
		std::sort(population.begin(), population.end());
		std::cout << "fitness = " << population[0].getFitness() << " | Improvement w.r.t. initial best solution = " << population[0].getFitness() - initialBestFitness << std::endl;
		population[0].print_path();
	}

	void printAll(){
		std::sort(population.begin(), population.end());		
		std::cout << "-------------------" << std::endl;
		int i = 0;
		for (auto it = population.begin(); it != population.end(); ++it)
		{
			std::cout << "fitness = " << (*it).getFitness() << std::endl;
			(*it).print_path();
			++i;
		}

		std::cout << "Tot el " << i << std::endl;
	}

	// This is the sequential version of "Evaluation" phase
	void AdjustPopulation(size_t const real_nPopulation){
		// insert all new generation items at the end of vector
		population.insert(population.end(), 
				std::make_move_iterator(nextGen.begin()),
				std::make_move_iterator(nextGen.end()));

		nextGen.clear();

		// we need to sort
		std::sort(population.begin(), 
						population.end());

		// erase all individuals after 'real_nPopulation'-th element
		while(population.size() > real_nPopulation)
			population.pop_back();

		// Recompute all needed parameters for the next algorithm's iteration
		totalFitness = std::accumulate(population.begin(), population.end(), 0.0, [](double acc, const Chromosome& a){
			return acc + (double) a.getFitness();
		});

		nPopulation = population.size();
		worst_fitness = population[nPopulation - 1].getFitness();
		best_fitness = population[0].getFitness();
	}

	int roulette_wheel_selection(){
		
		// Phase #1 - prepare for selection
		std::vector<double> mRankedWeight;
		size_t size = population.size();
		mRankedWeight.resize(size);

		double newFitness = (double) ((double) population[0].getFitness()) * (1.0 / totalFitness);
		mRankedWeight[0] = newFitness;

		for(size_t i = 1; i < size - 1; ++i){
			newFitness = (double) ((double) population[i].getFitness()) * (1.0 / totalFitness);
			mRankedWeight[i] = newFitness + mRankedWeight[i - 1];
		}

		mRankedWeight[size - 1] = 1.0;

		// Phase #2 - select
		double percent = r->random_percent();

		for(size_t i = 1; i < population.size(); i++)
			if(percent < mRankedWeight[i])
				return i - 1;

		return size - 1;
	}

	std::vector<Population> init_chunks_of_population(const int nw){
		int delta = nPopulation / nw, excess = 0;
		std::vector<Population> subpops;
		std::shared_ptr<Random> rSub, rSubAlt;


		// I want to pass the same random instance for all threads
		rSub.reset(new Random(nodes, delta, seed));	
		subpops.reserve(delta);

		// But if there's a chunk longer than delta, we have to create a different
		// random instance
		if((excess = nPopulation % delta) != 0) 
			rSubAlt.reset(new Random(nodes, delta + excess, seed));

		// shuffle to give heterogeneous parts to the threads
		std::random_shuffle(population.begin(), population.end());

		for(int i = 0; i < nw; i++){
			int start = i * delta;
			int end = (i != (nw - 1) ? (i + 1) * delta : nPopulation);

			std::vector<Chromosome> subV(population.begin() + start, population.begin() + end);

			if(end - start != delta){
				Population subP(subV, nodes, distances, rSubAlt);
				subpops.push_back(subP);
			}else{
				Population subP(subV, nodes, distances, rSub);
				subpops.push_back(subP);
			}
		}

		return subpops;
	}

	// reuse same Population class instances, changing only vector of chromosomes
	std::vector<std::vector<Chromosome>> get_chunks_of_population(const int nw){
		int delta = nPopulation / nw, excess = 0;
		std::vector<std::vector<Chromosome>> subpops;

		std::random_shuffle(population.begin(), population.end());

		for(int i = 0; i < nw; i++){
			int start = i * delta;
			int end = (i != (nw - 1) ? (i + 1) * delta : nPopulation);

			std::vector<Chromosome> subV(population.begin() + start, population.begin() + end);

			subpops.push_back(subV);
		}

		return subpops;
	}

	// This is the function to merge subpopulation's result
	void updatePopulation(std::vector<Population> v){
		int idx = 0;
		for(int i=0; i<v.size(); i++){
			auto el = v[i].getPopulation();
			population.insert(population.end(), 
				std::make_move_iterator(el.begin()),
				std::make_move_iterator(el.end()));
		}

		std::sort(population.begin(), 
						population.end());

		while(population.size() > nPopulation)
			population.pop_back();

		totalFitness = std::accumulate(population.begin(), population.end(), 0.0, [](double acc, const Chromosome& a){
			return acc + (double) a.getFitness();
		});

		nPopulation = population.size();
		worst_fitness = population[nPopulation - 1].getFitness();
		best_fitness = population[0].getFitness();

	}

};


#endif