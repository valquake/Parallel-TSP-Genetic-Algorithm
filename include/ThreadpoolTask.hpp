#ifndef THREADPOOL_H
#define THREADPOOL_H

#include <iostream>
#include <chrono>
#include <memory>
#include <vector>
#include <thread>
#include <atomic>
#include <future>
#include <functional>
#include <sched.h>

#include "../utils/queue.hpp"

#define EndStream nullptr

template <typename T>
struct ThreadpoolTask
{
	typedef std::shared_ptr<std::packaged_task<T()>> shared_task;

	ThreadpoolTask(const size_t num_w)
	: nw(num_w){

		queue.reset(new syque<shared_task>());
		
		for(size_t i = 0; i < nw; ++i){
            m_threads.emplace_back(std::thread(&ThreadpoolTask::threadroutine, this, i));
		}

        int max_threads = std::thread::hardware_concurrency();

        // Thread Sticking Technique
        for(size_t i = 0; i< nw; ++i) {
        	cpu_set_t cpuset;
        	CPU_ZERO(&cpuset);
        	CPU_SET(i % max_threads, &cpuset);
        	int rc = pthread_setaffinity_np(m_threads[i].native_handle(), sizeof(cpu_set_t), &cpuset);
        }
	}

	void threadroutine(const int id){
		while(true){
			shared_task t;
			
			// go to task queue and pop task
	        while((t = queue->pop()) != EndStream){
		 																																								
				static auto t0 = std::chrono::system_clock::now();
				auto t1 = std::chrono::system_clock::now();

				if(t->valid())
					(*t)();

	        	auto elapsedw = std::chrono::duration_cast<std::chrono::milliseconds>(t1-t0).count();
        	}


	        if(shutdown)
	        	break;	          
    	}
	}

	// function called by main thread to submit task to threadpool
	std::future<T> submit(shared_task &task) {
		auto f = task->get_future();

		queue->push(std::move(task));

		return f;
	}

	void sendEOS(){
		for (int i = 0; i < nw; ++i)
			queue->push(EndStream);
	}

	// When all is finished, shutdown threadpool
	void shutdownAll(){
		shutdown = true;
		sendEOS();
		for(auto& thread : m_threads)
            thread.join();

	}

	std::shared_ptr<syque<shared_task>>					queue;
	std::vector<std::thread> 							m_threads;
	bool 												shutdown = false;
	const size_t 										nw;	
};

#endif