#pragma once

class TwoHop
{
private:
	int __nodeId = -1, __seedSize = -1, __numNbr = 0, __numV;
	int *__pUpdateIndex, *__pNbrs;
	bool *__pBoolNbrs, *__pBoolSeed, __isOpt = true;
	double *__pActiveProbH1, *__pActiveProbH2, *__pActiveProbH2No;
	double *__pCopyActiveProbH1, *__pCopyActiveProbH2No;
	Nodelist __vecSeed;
	std::vector<std::pair<float, int>> __infHeap;
	const Graph __graph;
	PResult __pRes;
	CascadeModel __model;

	/// Build the heap.
	void build_heap();
	/// Build the heap under the IC model when optimization approach isn't used.
	void build_heap_nonopt_IC();
	/// Build the heap under the LT model when optimization approach isn't used.
	void build_heap_nonopt_LT();
	/// Copy the parameters (activation probabilities) for the two-hop neighbors.
	void copy_two_hop_nbrs(const int seedId) const;
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
	/// Compute the influence spread of a given seed set under the IC model.
	double comp_inf_spread_IC(const std::vector<int>& vecSeed) const;
	/// Compute the influence spread of a given seed set under the LT model.
	double comp_inf_spread_LT(const std::vector<int>& vecSeed) const;
public:
	TwoHop(const Graph& graph, const PResult& pRes, const CascadeModel model) : __graph(graph), __pRes(pRes),
	                                                                            __model(model)
	{
		__numV = (int)graph.size();
		__pUpdateIndex = (int *)calloc(__numV, sizeof(int));
		__pNbrs = (int *)calloc(__numV, sizeof(int));
		__pBoolNbrs = (bool *)malloc(__numV * sizeof(bool));
		memset(__pBoolNbrs, true, __numV);
		__pBoolSeed = (bool *)calloc(__numV, sizeof(bool));
		__pActiveProbH1 = (double *)calloc(__numV, sizeof(double));
		__pActiveProbH2 = (double *)calloc(__numV, sizeof(double));
		__pActiveProbH2No = (double *)malloc(__numV * sizeof(double));
		__pCopyActiveProbH1 = (double *)malloc(__numV * sizeof(double));
		__pCopyActiveProbH2No = (double *)malloc(__numV * sizeof(double));
		for (int i = 0; i < __numV; i++) __pActiveProbH2No[i] = 1.0;
		__infHeap = std::vector<std::pair<float, int>>(__numV);
	}

	~TwoHop()
	{
		free(__pUpdateIndex);
		free(__pNbrs);
		free(__pBoolNbrs);
		free(__pBoolSeed);
		free(__pActiveProbH1);
		free(__pActiveProbH2);
		free(__pActiveProbH2No);
		free(__pCopyActiveProbH1);
		free(__pCopyActiveProbH2No);
	}

	/// Set cascade model.
	void set_cascade_model(const CascadeModel model)
	{
		__model = model;
	}

	/// Set the method for building heap. True is to use optimization approach.
	void set_build_heap_method(const bool isOpt)
	{
		__isOpt = isOpt;
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

typedef TwoHop TTwoHop;
typedef std::shared_ptr<TTwoHop> PTwoHop;
