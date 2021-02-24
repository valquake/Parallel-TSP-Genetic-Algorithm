#include <iostream>
#include <random>
#include <thread>

namespace utils{

	/* Thread-safe function that returns a random number between min and max (inclusive).
	This function takes ~142% the time that calling rand() would take. For this extra
	cost you get a better uniform distribution and thread-safety. */
	int intRand(const int & min, const int & max, const int seed) {
	    static thread_local std::mt19937* generator = nullptr;
	    if (!generator) generator = new std::mt19937(seed);
	    std::uniform_int_distribution<int> distribution(min, max);
	    return distribution(*generator);
	}
	
	unsigned long long factorial(const unsigned int n){
		unsigned long long f = 1;

	    for(int i = 1; i <=n; ++i)
	    {
	        f *= i;
	    }

	    return f;
	}

	template <class InputIterator, class OutputIterator, class UnaryOperator, class Pred>
	OutputIterator transform_if(InputIterator first1, InputIterator last1,
	                            OutputIterator result, UnaryOperator op, Pred pred) {
		
	    while (first1 != last1) 
	    {
	        if (pred(*first1)) {
	            *result = op(*first1);
	            ++result;
	        }
	        ++first1;
	    }
	    return result;
	}

	template<class InputIt, class OutputIt, class BinaryOperation, class UnaryPredicate>
	OutputIt transform_if_cr(InputIt first, InputIt last, OutputIt d_first, BinaryOperation binary_op, UnaryPredicate pred)
	{
		if (first != last)
		{
			InputIt next=first; ++next;
			while (next != last) {
				if (pred(*first)) {
		        	*d_first++ = binary_op(*first, *next);
	        		*d_first++ = binary_op(*next, *first);
				}	
			  ++first; ++next;
			}
		}
		return d_first;
	}

}