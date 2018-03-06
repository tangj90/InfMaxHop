#pragma once

class GraphBase
{
public:
	static void check_graph(const std::string filename)
	{
		Graph vecG;
		TIO::load_file(filename, vecG);
		int nodeId = 0;
		for (auto& nbrs : vecG)
		{
			std::cout << nodeId++ << ": ";
			for (auto& nbr : nbrs)
			{
				std::cout << "(" << nbr.first << ", " << nbr.second << ") ";
			}
			std::cout << std::endl;
		}
	}

	/// Format the input for future computing, which is much faster for loading. Vector serialization is used.
	static void format_graph(const std::string filename, const std::string mode)
	{
		const char m1 = tolower(mode[0]);
		const bool cond = m1 == 'g' || m1 == 'w';
		if (!cond)
		{
			LogInfo("The input mode is not supported:", mode);
			LogInfo("The supported modes: -mode=g or -mode=w");
			LogInfo("g: graph only (default, using WC), w: with edge property");
		}

		int numV, numE;
		int srcId, dstId;
		float weight = 0.0;
		std::ifstream infile(filename);
		if (!infile.is_open())
		{
			std::cout << "The file \"" + filename + "\" can NOT be opened\n";
			return;
		}
		infile >> numV >> numE;

		// FILE* fin;
		// const errno_t err = fopen_s(&fin, filename.c_str(), "r");
		// if (err != 0)
		// {
		//	 std::cout << "The file \"" + filename + "\" can NOT be opened\n";
		//	 return;
		// }
		// fscanf_s(fin, "%d%d", &numV, &numE);
		Graph vecG(numV);
		std::vector<int> vecInDeg(numV);
		for (int i = numE; i--;)
		{
			if (m1 == 'w')
			{
				infile >> srcId >> dstId >> weight;
				// fscanf_s(fin, "%d%d%f", &srcId, &dstId, &weight);
			}
			else
			{
				infile >> srcId >> dstId;
				// fscanf_s(fin, "%d%d", &srcId, &dstId);
			}
			vecG[srcId].push_back(Edge(dstId, weight));
			vecInDeg[dstId]++;
		}
		infile.close();
		if (m1 == 'g')
		{
			// When the input is a graph only, set the diffusion probability using WC setting
			for (auto& nbrs : vecG)
			{
				for (auto& nbr : nbrs)
				{
					nbr.second = (float)1.0 / vecInDeg[nbr.first];
				}
			}
		}
		TIO::save_graph_struct(filename, vecG, false);
		std::cout << "The graph is formatted!" << std::endl;
	}

	/// Load graph via vector deserialization.
	static Graph load_graph(const std::string graphName, const std::string probDist = "load",
	                        const float probEdge = float(0.1))
	{
		Graph graph;
		TIO::load_graph_struct(graphName, graph, false);

		// Default is to load the edge including the diffusion probability from the file. The following changes the setting of diffusion probability.
		if (tolower(probDist[0]) == 'w')
		{// Weighted cascade
			std::vector<int> vecInDeg(graph.size());
			for (auto& nbrs : graph)
			{
				for (auto& nbr : nbrs)
				{
					vecInDeg[nbr.first]++;
				}
			}
			for (auto& nbrs : graph)
			{
				for (auto& nbr : nbrs)
				{
					nbr.second = (float)1.0 / vecInDeg[nbr.first];
				}
			}
		}
		else if (tolower(probDist[0]) == 't')
		{// Trivalency cascade
			dsfmt_gv_init_gen_rand(0);
			float pProb[3] = {float(0.001), float(0.01), float(0.1)};
			for (auto& nbrs : graph)
			{
				for (auto& nbr : nbrs)
				{
					nbr.second = pProb[dsfmt_gv_genrand_uint32_range(3)];
				}
			}
			dsfmt_gv_init_gen_rand(static_cast<uint32_t>(time(nullptr)));
		}
		else if (tolower(probDist[0]) == 'u')
		{// Uniform probability
			for (auto& nbrs : graph)
			{
				for (auto& nbr : nbrs)
				{
					nbr.second = probEdge;
				}
			}
		}
		return graph;
	}

	/// Evaluate influence spread using MC simulation.
	static double inf_eval(const Graph& graph, const std::vector<int>& vecSeed, const CascadeModel model,
	                       const int evalsize = 10000)
	{
		Timer EvalTimer;
		std::cout << "  >>>Evaluate influence...\n";
		int nodeId, currProgress = 0;
		std::queue<int> Que;
		std::vector<int> vecActivated;

		const auto numV = graph.size();
		double inf = (double)vecSeed.size();
		bool* activated = (bool *)calloc(numV, sizeof(bool));
		int* visited = (int *)calloc(numV, sizeof(int));
		std::vector<double> vecThr(numV);
		std::vector<double> vecActivateWeight(numV, 0.0);
		for (auto seedId : vecSeed) activated[seedId] = true;
		for (int i = 0; i < evalsize; i++)
		{
			if (i * 100 >= evalsize * currProgress)
			{
				const auto evalTime = EvalTimer.get_operation_time();
				if (evalTime > 100)
					std::cout << "\tMC-Progress at: " << currProgress << "%, " << "time used: " << evalTime << std::endl;
				currProgress += 20;
			}
			for (auto seed : vecSeed)
			{
				Que.push(seed);
			}

			// BFS traversal
			if (model == IC)
			{
				while (!Que.empty())
				{
					nodeId = Que.front();
					Que.pop();
					for (auto& nbr : graph[nodeId])
					{
						if (activated[nbr.first]) continue;
						if (dsfmt_gv_genrand_open_close() <= nbr.second)
						{
							activated[nbr.first] = true;
							vecActivated.push_back(nbr.first);
							Que.push(nbr.first);
						}
					}
				}
			}
			else if (model == LT)
			{
				while (!Que.empty())
				{
					nodeId = Que.front();
					Que.pop();
					for (auto& nbr : graph[nodeId])
					{
						if (activated[nbr.first]) continue;
						if (visited[nbr.first] < i + 1)
						{
							// First time visit this node
							visited[nbr.first] = i + 1;
							vecThr[nbr.first] = dsfmt_gv_genrand_open_close();
							vecActivateWeight[nbr.first] = 0.0;
						}
						vecActivateWeight[nbr.first] += nbr.second;
						if (vecActivateWeight[nbr.first] >= vecThr[nbr.first])
						{
							// Activation weight is greater than threshold
							activated[nbr.first] = true;
							vecActivated.push_back(nbr.first);
							Que.push(nbr.first);
						}
					}
				}
			}
			inf += (double)vecActivated.size() / evalsize;
			for (auto activatedNode : vecActivated) activated[activatedNode] = false;
			vecActivated.clear();
		}
		free(activated);
		free(visited);
		std::cout << "  >>>MC-Influence spread: " << inf << ", time used (sec): " << EvalTimer.get_total_time() << std::endl;
		return inf;
	}
};
