#include "stdafx.h"

void OneHop::build_heap()
{
	__nodeId = 0;
	for (auto& nbrs : __graph)
	{
		auto val = (float)1.0;
		for (auto& nbr : nbrs) val += nbr.second;
		__infHeap[__nodeId] = std::pair<float, int>(std::make_pair(val, __nodeId));
		__nodeId++;
	}
	make_max_heap(__infHeap);
}

double OneHop::cal_influence_add_node(const int seedId) const
{
	if (__model == IC)
	{
		return cal_influence_add_node_IC(seedId);
	}
	return cal_influence_add_node_LT(seedId);
}

double OneHop::cal_influence_add_node_IC(const int seedId) const
{
	double incInf = 1.0 - __pActiveProb[seedId];
	for (auto& nbr : __graph[seedId])
	{
		incInf += (1.0 - __pActiveProb[nbr.first]) * nbr.second;
	}
	return incInf;
}

double OneHop::cal_influence_add_node_LT(const int seedId) const
{
	double incInf = 1.0 - __pActiveProb[seedId];
	for (auto& nbr : __graph[seedId])
	{
		if (!__pBoolSeed[nbr.first]) incInf += nbr.second;
	}
	return incInf;
}

void OneHop::update_add_node(const int seedId) const
{
	if (__model == IC)
	{
		update_add_node_IC(seedId);
	}
	else
	{
		update_add_node_LT(seedId);
	}
}

void OneHop::update_add_node_IC(const int seedId) const
{
	__pActiveProb[seedId] = 1.0;
	for (auto& nbr : __graph[seedId])
	{
		__pActiveProb[nbr.first] += (1.0 - __pActiveProb[nbr.first]) * nbr.second;
	}
}

void OneHop::update_add_node_LT(const int seedId) const
{
	__pBoolSeed[seedId] = true;
	__pActiveProb[seedId] = 1.0;
	for (auto& nbr : __graph[seedId])
	{
		if (!__pBoolSeed[nbr.first]) __pActiveProb[nbr.first] += nbr.second;
	}
}

double OneHop::comp_inf_spread() const
{
	return comp_inf_spread(__vecSeed);
}

double OneHop::comp_inf_spread(const std::vector<int>& vecSeed) const
{
	memset(__pActiveProb, 0, sizeof(double) * __numV);
	for (const auto nodeId : vecSeed)
	{
		update_add_node(nodeId);
	}
	double sumInf = 0.0;
	for (int i = 0; i < __numV; i++)
	{
		sumInf += __pActiveProb[i];
	}
	return sumInf;
}

void OneHop::greedy(const int seedSize)
{
	Timer run_timer;
	__seedSize = 0;
	int roundIndex = 0;
	double sumInf = 0.0;

	build_heap();
	__vecSeed = Nodelist(seedSize);
	while (__seedSize < seedSize)
	{
		const auto checkNode = __infHeap.front();
		if (__pUpdateIndex[checkNode.second] == roundIndex)
		{
			pop_heap(__infHeap.begin(), __infHeap.end());
			__infHeap.pop_back();
			update_add_node(checkNode.second);
			__vecSeed[__seedSize++] = checkNode.second;
			sumInf += checkNode.first;
			roundIndex++;
		}
		else
		{
			__pRes->inc_estimated_node_size(1);
			auto val = (float)cal_influence_add_node(checkNode.second);

			max_heap_replace_max_value(__infHeap, val);
			__pUpdateIndex[checkNode.second] = roundIndex;
		}
	}
	__pRes->set_running_time(run_timer.get_total_time());
	__pRes->set_influence_original(sumInf);
	__pRes->set_seed_vec(__vecSeed);
}
