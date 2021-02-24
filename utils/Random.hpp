#ifndef RANDOM
#define RANDOM

#include <random>
#include <mutex>
#include <memory>
#include <algorithm>
#include <iostream>

typedef std::mt19937 rng_type;

class Random
{

    private:
		rng_type rng;
		std::random_device rd;
	    std::uniform_int_distribution<rng_type::result_type> ndist;
	    std::uniform_int_distribution<rng_type::result_type> pdist;
		std::uniform_int_distribution<rng_type::result_type> d;
		std::uniform_real_distribution<double> unif;
		std::shared_ptr<std::mutex> d_mutex;
		
	public:
		Random(int n, int p, int seed){
		    std::uniform_int_distribution<rng_type::result_type> ndistx(0, n-1);
		    std::uniform_int_distribution<rng_type::result_type> pdistx(0, p-1);
		    std::uniform_int_distribution<rng_type::result_type> dx(1, 100);
		    std::uniform_real_distribution<double> unifx(0.0, 1.0);
		    
		    ndist = ndistx;
		    pdist = pdistx;
		    d = dx;
		    unif = unifx;

		    d_mutex.reset(new std::mutex());
			rng.seed(seed);
		};

		int random_node(){
			std::unique_lock<std::mutex> l(*d_mutex);
		    rng_type::result_type random_number = ndist(rng);
		    return random_number;
		}

		int random_node(int n){
			std::unique_lock<std::mutex> l(*d_mutex);
		    rng_type::result_type random_number = ndist(rng, decltype(ndist)::param_type(n, ndist.max()));
			return random_number;
		}


		int random_number(){
		    std::unique_lock<std::mutex> l(*d_mutex);
		    rng_type::result_type random_number = d(rng);
		    return random_number;
		}

		void rand_shuffle(std::vector<int> &v){
			std::unique_lock<std::mutex> l(*d_mutex);
			rng_type urng(rd());
			std::shuffle(v.begin(), v.end(), urng);
		}

		double random_percent(){
			std::unique_lock<std::mutex> l(*d_mutex);
			double random_number = unif(rng);

			return random_number;
		}
};

#endif