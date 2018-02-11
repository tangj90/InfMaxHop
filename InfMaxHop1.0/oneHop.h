#pragma once

class OneHop
{
private:
	bool* __pBoolSeed;
	int __nodeId = -1, __seedSize = -1, __numV, *__pUpdateIndex;
	double* __pActiveProb;
	Nodelist __vecSeed;
	std::vector<std::pair<float, int>> __infHeap;
	const Graph __graph;
	PResult __pRes;
	CascadeModel __model;

	/// Build the heap.
	void build_heap();
	/// Calculate the marginal influence gain of adding a new seed.
	double cal_influence_add_node(const int seedId) const;
	/// Calculate the marginal influence gain of adding a new seed under the IC model.
	double cal_influence_add_node_IC(const int seedId) const;
	/// Calculate the marginal influence gain of adding a new seed under the LT model.
	double cal_influence_add_node_LT(const int seedId) const;
	/// Update the parameters of adding a new seed.
	void update_add_node(const int seedId) const;
	/// Update the parameters of adding a new seed under the IC model.
	void update_add_node_IC(const int seedId) const;
	/// Update the parameters of adding a new seed under the IC model.
	void update_add_node_LT(const int seedId) const;
public:
	OneHop(const Graph& graph, const PResult& pRes, const CascadeModel model) : __graph(graph), __pRes(pRes),
	                                                                            __model(model)
	{
		__numV = (int)__graph.size();
		__pBoolSeed = (bool *)calloc(__numV, sizeof(bool));
		__pUpdateIndex = (int *)calloc(__numV, sizeof(int));
		__pActiveProb = (double *)calloc(__numV, sizeof(double));
		__infHeap = std::vector<std::pair<float, int>>(__numV);
	}

	~OneHop()
	{
		free(__pBoolSeed);
		free(__pUpdateIndex);
		free(__pActiveProb);
	}

	/// Set cascade model.
	void set_cascade_model(const CascadeModel model)
	{
		__model = model;
	}

	/// Get the results.
	PResult get_result() const
	{
		return __pRes;
	}

	/// Compute the influence spread of the seed set constructed.
	double comp_inf_spread() const;
	/// Compute the influence spread of a given seed set.
	double comp_inf_spread(const std::vector<int>& vecSeed) const;
	/// The main seed selection algorithm by a greedy procedure.
	void greedy(const int seedSize);
};

using TOneHop = OneHop;
using POneHop = std::shared_ptr<TOneHop>;
