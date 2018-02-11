#pragma once

/// Log information
template <typename _Ty>
static inline void LogInfo(_Ty val)
{
	std::cout << val << std::endl;
}

/// Log information
template <typename _Ty>
static inline void LogInfo(std::string title, _Ty val)
{
	std::cout << title << ": " << val << std::endl;
}

/// Math, pow2
static inline double pow2(double t)
{
	return t * t;
}

/// Math, log2
static inline double log2(int n)
{
	return log(n) / log(2);
}

/// Math, logcnk
static inline double logcnk(int n, int k)
{
	k = min(k, n - k);
	double res = 0;
	for (int i = 1; i <= k; i++) res += log(double(n - k + i) / i);
	return res;
}

/// Generate one node with probabilities according to their weights
static inline int gen_random_node_by_weight(const int numV, const double* pAccumWeight = nullptr)
{
	if (pAccumWeight == nullptr) return dsfmt_gv_genrand_uint32_range(numV);
	const double weight = dsfmt_gv_genrand_open_close();
	int minIdx = 0, maxIdx = numV - 1;
	if (weight < pAccumWeight[0]) return 0;
	while (maxIdx > minIdx)
	{
		const int meanIdx = (minIdx + maxIdx) / 2;
		const auto meanWeight = pAccumWeight[meanIdx];
		if (weight <= meanWeight) maxIdx = meanIdx;
		else minIdx = meanIdx + 1;
	}
	return maxIdx;
}

/// Make the vector to a max-heap.
static inline void make_max_heap(std::vector<std::pair<float, int>>& vec)
{
	// Max heap
	const auto size = vec.size();
	if (2 <= size)
	{
		for (auto hole = (size + 1) / 2; hole--;)
		{
			const auto val = vec[hole];
			size_t i, child;
			for (i = hole; i * 2 + 1 < size; i = child)
			{
				// Find smaller child
				child = i * 2 + 2;
				if (child == size || vec[child - 1] > vec[child])
				{
					// One child only or the left child is greater than the right one
					--child;
				}

				// Percolate one level
				if (vec[child] > val)
				{
					vec[i] = vec[child];
				}
				else
				{
					break;
				}
			}
			vec[i] = val;
		}
	}
}

/// Replace the value for the first element and down-heap this element.
static inline void max_heap_replace_max_value(std::vector<std::pair<float, int>>& vec, const float& val)
{
	// Increase the value of the first element
	const auto size = vec.size();
	size_t i, child;
	auto hole = vec[0];
	for (i = 0; i * 2 + 1 < size; i = child)
	{
		// Find smaller child
		child = i * 2 + 2;
		if (child == size || vec[child - 1] > vec[child])
		{
			// One child only or the left child is greater than the right one
			--child;
		}

		// Percolate one level
		if (vec[child].first > val)
		{
			vec[i] = vec[child];
		}
		else
		{
			break;
		}
	}
	hole.first = val;
	vec[i] = hole;
}
