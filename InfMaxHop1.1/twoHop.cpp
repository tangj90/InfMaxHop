#include "stdafx.h"

void TwoHop::build_heap()
{
	if (!__isOpt)
	{
		// Build the heap via the exact influence spread of each single node
		if (__model == IC) build_heap_nonopt_IC();
		else build_heap_nonopt_LT();
		return;
	}
	__nodeId = 0;
	for (auto& nbrs : __graph)
	{
		auto val = 1.0;
		for (auto& nbr : nbrs)
		{
			val += nbr.second;
		}
		__pActiveProbH1[__nodeId++] = val;
	}
	__nodeId = 0;
	for (auto& nbrs : __graph)
	{
		auto sumInf = 1.0;
		for (auto& nbr : nbrs)
		{
			sumInf += nbr.second * __pActiveProbH1[nbr.first];
		}
		__infHeap[__nodeId] = std::pair<float, int>(std::make_pair((float)sumInf, __nodeId));
		__nodeId++;
	}
	make_max_heap(__infHeap);
	memset(__pActiveProbH1, 0, __numV * sizeof(double));
}

void TwoHop::build_heap_nonopt_IC()
{
	Timer timer;
	for (int i = 0; i < __numV; i++) __pActiveProbH2No[i] = 1.0;
	int numNbr = 0;
	std::vector<int> vecNbr(__numV);
	int cc = 0;
	LogInfo("  -->Build heap when optimization isn't used.");
	for (int seedId = 0; seedId < __numV; seedId++)
	{
		// One hop neighbor and the activation probability
		__pActiveProbH2No[seedId] = 0.0;
		__pBoolNbrs[seedId] = false;
		vecNbr[numNbr++] = seedId;
		for (auto& nbr : __graph[seedId])
		{
			if (__pBoolNbrs[nbr.first])
			{
				__pBoolNbrs[nbr.first] = false;
				vecNbr[numNbr++] = nbr.first;
			}
			/*Hop two*/
			__pActiveProbH2No[nbr.first] *= (1.0 - nbr.second);
			for (auto& twoNbr : __graph[nbr.first])
			{
				if (__pBoolNbrs[twoNbr.first])
				{
					__pBoolNbrs[twoNbr.first] = false;
					vecNbr[numNbr++] = twoNbr.first;
				}
				__pActiveProbH2No[twoNbr.first] *= (1.0 - twoNbr.second * nbr.second);
			}
		}
		double sumInf = numNbr;
		while (numNbr)
		{
			const auto nbrId = vecNbr[--numNbr];
			sumInf -= __pActiveProbH2No[nbrId];
			__pBoolNbrs[nbrId] = true;
			__pActiveProbH2No[nbrId] = 1.0;
		}
		__infHeap[seedId] = std::pair<float, int>(std::make_pair((float)sumInf, seedId));
		if (cc++ % 1000000 == 0)
		{
			std::cout << "  -->" << cc << " nodes processed: " << timer.get_operation_time() << ", ";
			std::cout << timer.get_total_time() << std::endl;
		}
	}
	make_max_heap(__infHeap);
	LogInfo("  -->Build heap finished", timer.get_total_time());
}

void TwoHop::build_heap_nonopt_LT()
{
	Timer timer;
	int cc = 0;
	LogInfo("  -->Build heap when optimization isn't used.");
	for (int seedId = 0; seedId < __numV; seedId++)
	{
		// One hop neighbor and the activation probability
		auto sumInf = 1.0;
		for (auto& nbr : __graph[seedId])
		{
			if (__pBoolSeed[nbr.first]) continue;
			/*Hop two*/
			auto incInf = 1.0;
			for (auto& twoNbr : __graph[nbr.first])
			{
				if (twoNbr.first != seedId) incInf += twoNbr.second;
			}
			sumInf += nbr.second * incInf;
		}
		__infHeap[seedId] = std::pair<float, int>(std::make_pair((float)sumInf, seedId));
		if (cc++ % 1000000 == 0)
		{
			std::cout << "  -->" << cc << " nodes processed: " << timer.get_operation_time() << ", ";
			std::cout << timer.get_total_time() << std::endl;
		}
	}
	make_max_heap(__infHeap);
	LogInfo("  -->Build heap finished", timer.get_total_time());
}

void TwoHop::copy_two_hop_nbrs(const int seedId) const
{
	int numNbr = 0;
	for (auto& nbr : __graph[seedId])
	{
		if (__pBoolNbrs[nbr.first])
		{
			__pNbrs[numNbr++] = nbr.first;
			__pBoolNbrs[nbr.first] = false;
		}
		for (auto& twoNbr : __graph[nbr.first])
		{
			if (__pBoolNbrs[twoNbr.first])
			{
				__pNbrs[numNbr++] = twoNbr.first;
				__pBoolNbrs[twoNbr.first] = false;
			}
		}
	}
	while (numNbr)
	{
		// Copy the values of all the two-hop neighbors
		const auto& nodeId = __pNbrs[--numNbr];
		__pCopyActiveProbH1[nodeId] = __pActiveProbH1[nodeId];
		__pCopyActiveProbH2No[nodeId] = __pActiveProbH2No[nodeId];
		__pBoolNbrs[nodeId] = true;
	}
}

double TwoHop::cal_influence_add_node(const int seedId) const
{
	if (__model == IC)
	{
		return cal_influence_add_node_IC(seedId);
	}
	return cal_influence_add_node_LT(seedId);
}

double TwoHop::cal_influence_add_node_IC(const int seedId) const
{
	copy_two_hop_nbrs(seedId);
	auto incInf = __pCopyActiveProbH2No[seedId];
	const auto seedInfH1 = __pCopyActiveProbH1[seedId];
	__pCopyActiveProbH1[seedId] = 1.0;
	__pCopyActiveProbH2No[seedId] = 0.0;
	for (auto& nbr : __graph[seedId])
	{
		const auto infNode = __pCopyActiveProbH1[nbr.first];
		const auto infNodeNew = infNode + nbr.second - infNode * nbr.second;
		if (abs(infNodeNew - infNode) < 1e-6) continue;
		__pCopyActiveProbH1[nbr.first] = infNodeNew;
		/*Hop two*/
		auto twoInfNodeNo = __pCopyActiveProbH2No[nbr.first];
		if (abs(twoInfNodeNo) > 1e-6)
		{
			__pCopyActiveProbH2No[nbr.first] *= (1.0 - nbr.second * 1.0) / (1.0 - nbr.second * seedInfH1);
			incInf += twoInfNodeNo - __pCopyActiveProbH2No[nbr.first];
		}
		for (auto& twoNbr : __graph[nbr.first])
		{
			twoInfNodeNo = __pCopyActiveProbH2No[twoNbr.first];
			if (abs(twoInfNodeNo) > 1e-6)
			{
				__pCopyActiveProbH2No[twoNbr.first] *= (1.0 - twoNbr.second * infNodeNew) / (1.0 - twoNbr.second * infNode);
				incInf += twoInfNodeNo - __pCopyActiveProbH2No[twoNbr.first];
			}
		}
	}
	return incInf;
}

double TwoHop::cal_influence_add_node_LT(const int seedId) const
{
	__pBoolSeed[seedId] = true;
	double incInf = 1.0 - __pActiveProbH2[seedId];
	const double prob = 1.0 - __pActiveProbH1[seedId];
	for (auto& nbr : __graph[seedId])
	{
		if (__pBoolSeed[nbr.first]) continue;
		incInf += prob * nbr.second;
		/*Hop two*/
		for (auto& twoNbr : __graph[nbr.first])
		{
			if (__pBoolSeed[twoNbr.first]) continue;
			incInf += nbr.second * twoNbr.second;
		}
	}
	__pBoolSeed[seedId] = false;
	return incInf;
}

void TwoHop::update_add_node(const int seedId) const
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

void TwoHop::update_add_node_IC(const int seedId) const
{
	const auto seedInfH1 = __pActiveProbH1[seedId];
	__pActiveProbH1[seedId] = 1.0;
	__pActiveProbH2No[seedId] = 0.0;
	for (auto& nbr : __graph[seedId])
	{
		const auto infNode = __pActiveProbH1[nbr.first];
		const auto infNodeNew = infNode + nbr.second - infNode * nbr.second;
		if (abs(infNodeNew - infNode) < 1e-6) continue;
		__pActiveProbH1[nbr.first] = infNodeNew;
		/*Hop two*/
		const auto twoInfNodeNo = __pActiveProbH2No[nbr.first];
		if (abs(twoInfNodeNo) > 1e-6)
		{
			__pActiveProbH2No[nbr.first] = twoInfNodeNo * (1.0 - nbr.second * 1.0) / (1.0 - nbr.second * seedInfH1);
		}
		for (auto& twoNbr : __graph[nbr.first])
		{
			__pActiveProbH2No[twoNbr.first] *= (1.0 - twoNbr.second * infNodeNew) / (1.0 - twoNbr.second * infNode);
		}
	}
}

void TwoHop::update_add_node_LT(const int seedId) const
{
	__pBoolSeed[seedId] = true;
	const double prob = 1.0 - __pActiveProbH1[seedId];
	__pActiveProbH1[seedId] = 1.0;
	__pActiveProbH2[seedId] = 1.0;
	for (auto& nbr : __graph[seedId])
	{
		if (__pBoolSeed[nbr.first]) continue;
		__pActiveProbH1[nbr.first] += nbr.second;
		__pActiveProbH2[nbr.first] += prob * nbr.second;
		/*Hop two*/
		for (auto& twoNbr : __graph[nbr.first])
		{
			if (__pBoolSeed[twoNbr.first]) continue;
			__pActiveProbH2[twoNbr.first] += nbr.second * twoNbr.second;
		}
	}
}

double TwoHop::comp_inf_spread_IC(const std::vector<int>& vecSeed) const
{
	memset(__pActiveProbH1, 0, sizeof(double) * __numV);
	for (int i = 0; i < __numV; i++) __pActiveProbH2No[i] = 1.0;
	for (const auto& seedId : vecSeed)
	{
		update_add_node_IC(seedId);
	}
	double sumInf = __numV;
	for (int i = 0; i < __numV; i++)
	{
		sumInf -= __pActiveProbH2No[i];
		__pActiveProbH1[i] = 0.0;
		__pActiveProbH2No[i] = 1.0;
	}
	return sumInf;
}

double TwoHop::comp_inf_spread_LT(const std::vector<int>& vecSeed) const
{
	memset(__pBoolSeed, 0, sizeof(bool) * __numV);
	memset(__pActiveProbH1, 0, sizeof(double) * __numV);
	for (auto seedId : vecSeed)
	{
		__pBoolSeed[seedId] = true;
		__pActiveProbH1[seedId] = 1.0;
	}
	std::vector<bool> vecBoolNbr(__numV, false);
	std::vector<int> vecIntNbr(__numV);
	int numNbr = 0;
	double sumInf = (double)vecSeed.size();
	for (auto seedId : vecSeed)
	{
		// One hop neighbor and the activation probability
		for (auto& nbr : __graph[seedId])
		{
			if (__pBoolSeed[nbr.first]) continue;
			__pActiveProbH1[nbr.first] += nbr.second;
			if (!vecBoolNbr[nbr.first])
			{
				vecBoolNbr[nbr.first] = true;
				vecIntNbr[numNbr++] = nbr.first;
			}
		}
	}
	while (numNbr)
	{
		// one hop activation probability * one hop influence spread
		double incInf = 1.0;
		const auto nodeId = vecIntNbr[--numNbr];
		for (auto& nbr : __graph[nodeId])
		{
			if (__pBoolSeed[nbr.first]) continue;
			incInf += nbr.second;
		}
		sumInf += __pActiveProbH1[nodeId] * incInf;
	}
	memset(__pBoolSeed, 0, sizeof(bool) * __numV);
	memset(__pActiveProbH1, 0, sizeof(double) * __numV);
	return sumInf;
}

double TwoHop::comp_inf_spread() const
{
	return comp_inf_spread(__vecSeed);
}

double TwoHop::comp_inf_spread(const std::vector<int>& vecSeed) const
{
	if (__model == IC) return comp_inf_spread_IC(vecSeed);
	return comp_inf_spread_LT(vecSeed);
}

void TwoHop::greedy(const int seedSize)
{
	Timer run_timer;
	__seedSize = 0;
	int roundIndex = 1, currProgress = 0;
	double sumInf = 0.0;
	build_heap();
	//run_timer.log_operation_time("build heap");
	__vecSeed = Nodelist(seedSize);
	while (__seedSize < seedSize)
	{
		if (__seedSize * 100 >= seedSize * currProgress)
		{
			const auto evalTime = run_timer.get_total_time();
			if (evalTime > 100)
			{
				std::cout << "\tH2 progress at: " << currProgress << "%, " << "total time used: " << evalTime << std::endl;
			}
			currProgress += 10;
			continue;
		}
		const auto checkNode = __infHeap.front();
		//std::cout << checkNode.first << " " << checkNode.second;
		if (__pUpdateIndex[checkNode.second] == roundIndex)
		{
			//std::cout << " true" << '\n';
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
			//std::cout << " " << val << " false" << '\n';
			max_heap_replace_max_value(__infHeap, val);
			__pUpdateIndex[checkNode.second] = roundIndex;
		}
	}
	__pRes->set_running_time(run_timer.get_total_time());
	__pRes->set_influence_original(sumInf);
	__pRes->set_seed_vec(__vecSeed);
}
