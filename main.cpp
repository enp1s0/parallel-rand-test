#include <iostream>
#include <random>
#include <vector>
#include <chrono>
#include <typeinfo>
#include <climits>
#include <omp.h>

constexpr std::size_t array_len = 1lu << 20;

struct mt {
	std::mt19937 _mt;
	mt(const std::uint64_t seed) : _mt(seed) {}
	std::uint64_t operator() () {return _mt();}
	std::uint64_t max() {return _mt.max();}
	std::uint64_t min() {return _mt.min();}
};

struct xorwow {
	std::uint64_t stat;
	xorwow(const std::uint64_t seed) : stat(seed) {}
	std::uint64_t operator() () {
		std::uint64_t x = stat;
		x ^= x << 13;
		x ^= x >> 7;
		x ^= x << 17;
		return stat = x;
	}
	std::uint64_t max() {return std::numeric_limits<std::uint64_t>::max();}
	std::uint64_t min() {return std::numeric_limits<std::uint64_t>::min();}
};

template <class Rand>
struct parallel_rand_1 {
	void operator() (
		double* const ptr,
		const std::size_t len
		) {
		std::uniform_real_distribution<double> dist(-1, 1);

#pragma omp parallel for
		for (std::size_t i = 0; i < len; i++) {
			Rand rnd(i);
			ptr[i] = dist(rnd);
		}
	}

	std::string get_name() const {
		return std::string(typeid(*this).name()) + "<" + std::string(typeid(Rand).name()) + ">";
	}
};

template <class Rand>
struct parallel_rand_2 {
	void operator() (
		double* const ptr,
		const std::size_t len
		) {
		std::uniform_real_distribution<double> dist(-1, 1);

#pragma omp parallel
		{
			Rand rnd(omp_get_thread_num());
			for (std::size_t i = omp_get_thread_num(); i < len; i += omp_get_num_threads()) {
				ptr[i] = dist(rnd);
			}
		}
	}

	std::string get_name() const {
		return std::string(typeid(*this).name()) + "<" + std::string(typeid(Rand).name()) + ">";
	}
};

template <class ParallelRand>
void measure(
	const std::size_t len
	) {
	std::vector<double> array(len);
	ParallelRand pr;

	const auto start_clock = std::chrono::system_clock::now();
	pr(array.data(), len);
	const auto end_clock = std::chrono::system_clock::now();
	const auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end_clock - start_clock).count() * 1e-9;

	std::printf("%s,%e,%e\n", pr.get_name().c_str(), elapsed_time,
							1. * len * sizeof(double) / elapsed_time);
}

int main() {
	std::printf("mode,time,throughput_in_byte_per_sec\n");
	measure<parallel_rand_1<mt    >>(array_len);
	measure<parallel_rand_2<mt    >>(array_len);
	measure<parallel_rand_1<xorwow>>(array_len);
	measure<parallel_rand_2<xorwow>>(array_len);
}
