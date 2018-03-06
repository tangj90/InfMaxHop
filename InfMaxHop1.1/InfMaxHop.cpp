/**
* @file InfMaxHop.cpp
* @brief This project implements the hop-based influence maximization algorithms for the following papers:
* Jing Tang, Xueyan Tang, Junsong Yuan, "An Efficient and Effective Hop-Based Approach for Influence Maximization in Social Networks," in Social Network Analysis and Mining, 2018.
* Jing Tang, Xueyan Tang, Junsong Yuan, "Influence Maximization Meets Efficiency and Effectiveness: A Hop-Based Approach," in Proc. IEEE/ACM ASONAM, 2017, pp. 64–71.
*
* @author Jing Tang (Nanyang Technological University)
*
* Copyright (C) 2018 Jing Tang and Nanyang Technological University. All rights reserved.
*
*/

#include "stdafx.h"
// #include "SFMT/dSFMT/dSFMT.c"
// #include "oneHop.cpp"
// #include "twoHop.cpp"

int main(int argc, char* argv[])
{
	// Randomize the seed for generating random numbers
	dsfmt_gv_init_gen_rand(static_cast<uint32_t>(time(nullptr)));
	TArgument Arg(argc, argv);
	const std::string infilename = Arg._dir + "/" + Arg._graphname;
	if (Arg._func == 0 || Arg._func == 2)
	{
		// Format the graph
		GraphBase::format_graph(infilename, Arg._mode);
		if (Arg._func == 0) return 1;
	}

	std::cout << "---The Begin of " << Arg._outFileName << "---" << std::endl;
	Timer mainTimer("main");
	// Initialize a result object to record the results
	PResult pResult(new TResult());
	// Load the graph
	const Graph graph = GraphBase::load_graph(infilename, Arg._probDist, Arg._probEdge);
	//int numV = (int)graph.size();	
	std::cout << "  ==>Graph loaded for RIS! total time used (sec): " + std::to_string(mainTimer.get_total_time()) << std::
		endl;

	auto alg = tolower(Arg._algName[0]);
	if (alg == 'o')
	{
		// OneHop algorithm
		TOneHop tAlg(graph, pResult, Arg._model);
		tAlg.greedy(Arg._seedsize);
	}
	else if (alg == 't')
	{
		// TwoHop algorithm
		TTwoHop tAlg(graph, pResult, Arg._model);
		if (!Arg._opt) tAlg.set_build_heap_method(Arg._opt);
		tAlg.greedy(Arg._seedsize);
	}
	else
	{
		LogInfo("\nERROR! The input algorithm is invalid. Supported algorithms: oneHop and twoHop.");
		return -1;
	}
	LogInfo("  >>>Selection finished! time used (sec)", pResult->get_running_time());
	const auto inf = GraphBase::inf_eval(graph, pResult->get_seed_vec(), Arg._model);
	pResult->set_influence(inf);
	LogInfo("  >>>Evaluation finished! Print results:");
	TIO::write_result(Arg._outFileName, pResult, Arg._resultFolder);
	TIO::write_order_seeds(Arg._outFileName, pResult, Arg._resultFolder);
	std::cout << "---The End of " << Arg._outFileName << "---" << std::endl;
	return 0;
}
