#include <iostream>
#include <SoapySDR/Device.hpp>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <fstream>
#include <ctime>
#include <chrono>


class radio{
	
	private:
	// constructor parameters
	double r_gain;
	long double r_sampling_rate;
	double r_frequency;

	// acquireWriteBuffer parameters
	int r_flags;
	long long r_timeNs;
	int r_timeout = 100000;
	int r_numelems = 1024;
	
	//startStreaming parameters
	std::queue<std::complex<float>*>* output_queue;
	std::mutex* output_mutex;
	std::condition_variable* output_var;
	std::thread* stream_thread;
	
	size_t handle;	// idex value used in the release call
	std::vector<size_t> channels{0}; // only 1 channel for the NooELEC
	bool run = false;
	std::ofstream error;
	
	
	SoapySDR::Device *device; // radio declaration
	SoapySDR::Stream *data_stream; // tranmission stream declasration
	

	public:

	// constructor
	radio(double gain, double sampling_rate, double frequency, std::queue<std::complex<float>*>* q);

	// member function prototypes
	void streamer();	
	void stopStreaming();
	void startStreaming(std::queue<std::complex<float>*>* queue, std::mutex& mutex, std::condition_variable& var);
	
};
