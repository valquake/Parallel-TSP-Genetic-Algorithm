#ifndef CHROMO_H
#define CHROMO_H

#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>
#include <set>
#include <tuple>

#include "symm.hpp"
#include "../utils/utils.cpp"
#include "../utils/Random.hpp"

struct Chromosome
{
		std::vector<int> chromosome; // the chromosome
		std::shared_ptr<SymmMatrix> distances; // shared ptr to matrix distances
		int fitness, nodes; // fitness value and #nodes

		Chromosome(const size_t n, std::shared_ptr<SymmMatrix> m, std::shared_ptr<Random> rand): nodes(n), distances(std::move(m)){
			chromosome.reserve(n);

			// Fill the vector with the values 
		    // 1, 2, 3, ..., n 
		    for (int i = 0; i < nodes; i++) 
		       	chromosome.push_back(i); 

			// Shuffle
	  		rand->rand_shuffle(chromosome);

	  		// Fitness computation
	  		fitness = 0;
		    for (int i = 0; i < nodes-1; i++) 
		       	fitness += distances->getDistance(chromosome[i], chromosome[i+1]);

		    fitness += distances->getDistance(chromosome[nodes - 1], chromosome[0]);
		};
		Chromosome(const size_t n, std::shared_ptr<SymmMatrix> m, std::vector<int> c): nodes(n), distances(std::move(m)), chromosome(std::move(c)){
			fitness = 0;
		    for (int i = 0; i < nodes-1; i++) 
		       	fitness += distances->getDistance(chromosome[i], chromosome[i+1]);

	        fitness += distances->getDistance(chromosome[nodes - 1], chromosome[0]);
		};

		// comparison operator (needed for sort algorithm)
		bool operator<(Chromosome const& r) const
	    {
	    	return std::tie(fitness, chromosome) < std::tie(r.fitness, r.chromosome);
	    }

	    // other useful comparators
	    bool operator==(Chromosome const& rhs) const { 
	    	std::vector<int> s = rhs.getChromosome();

	    	return chromosome == s;
		}

	    bool operator!=(Chromosome const& rhs) { 
	    	std::vector<int> s = rhs.getChromosome();

	    	return !(chromosome == s);
		}

		// return a copy of vector permutation
		std::vector<int> getChromosome() const {
			return chromosome;
		}

		int getFitness() const { 
			return fitness;
		}

	  	void print_path() const{
	  		for(size_t i=0; i<chromosome.size(); i++)
	  			std::cout << chromosome[i]  << " ";

	  		cout << endl;
	  	}

	  	// returns the chromosome resulting from mutation
	  	Chromosome mutation(std::shared_ptr<Random> &r){
			// pick two random elements
			// pick a random element
			int first = r->random_node(0); //utils::intRand(0, nodes - 2, seed); //(rand () % (chromosome.size() - 1));
			if(first == nodes - 1)
				first -= 2;

		  	// pick a random element from what's left (there is one fewer to choose from)...
		  	int second = r->random_node(first + 1); //utils::intRand(first + 1, nodes - 1, seed);//(rand () % (chromosome.size() - first)) + first;

		  	if(second == first + 1){
		  		if(second != nodes - 1)
		  			second ++;
		  		else
		  			first --;
		  	}

		  	std::vector<int> newVec(chromosome);

		  	// swapping
	  		std::swap(newVec[first], newVec[second]);

	  		// create a new chromosome with this new permutation
	  		Chromosome newChr = Chromosome(nodes, distances, std::move(newVec));

	  		return newChr;

	  	}

	  	void find_elements(std::vector<int> &v, std::vector<int> chr){
	  		int index = 0;
	  		for(std::size_t i = 0; i < chr.size(); ++i) {
			    auto it = std::find(v.begin(), v.end(), chr[i]); 
			    if (it != v.end()) {
			        std::swap(v[std::distance(v.begin(), it)], v[index]); 
			        index++;

			        if(index == v.size() - 1)
			        	break;
			    }
			}
	  	}

	  	// returns the chromosome resulting from crossover
	  	Chromosome crossover(const int first, const int second, std::vector<int> chr){
	  		std::vector<int> newVec(chromosome);
	  		auto p1 = newVec.cbegin() + first;
	  		auto p2 = newVec.cbegin() + second + 1;
	  		std::vector<int> v(p1, p2);
	  		
	  		find_elements(v, chr);

   			std::copy(v.begin(), v.end(), newVec.begin() + first); 
   			Chromosome newChr = Chromosome(nodes, distances, std::move(newVec));

   			return newChr;
	  	}


};


#endif