#pragma once

class IOcontroller
{
public:
	/// Save a serialized file
	template <class T>
	static void save_file(const std::string filename, const T& output)
	{
		std::ofstream outfile(filename, std::ios::binary);
		if (!outfile.eof() && !outfile.fail())
		{
			StreamType res;
			serialize(output, res);
			outfile.write(reinterpret_cast<char*>(&res[0]), res.size());
			outfile.close();
			res.clear();
			std::cout << "Save file successfully: " << filename << '\n';
		}
		else
		{
			std::cout << "Save file failed: " + filename << '\n';
			exit(1);
		}
	}

	/// Load a serialized file
	template <class T>
	static void load_file(const std::string filename, T& input)
	{
		std::ifstream infile(filename, std::ios::binary);
		if (!infile.eof() && !infile.fail())
		{
			infile.seekg(0, std::ios_base::end);
			const std::streampos fileSize = infile.tellg();
			infile.seekg(0, std::ios_base::beg);
			std::vector<uint8_t> res(fileSize);
			infile.read(reinterpret_cast<char*>(&res[0]), fileSize);
			infile.close();
			input.clear();
			auto it = res.cbegin();
			input = deserialize<T>(it, res.cend());
			res.clear();
		}
		else
		{
			std::cout << "Cannot open file: " + filename << '\n';
			exit(1);
		}
	}

	/// Save graph structure to a file
	static void save_graph_struct(const std::string graphName, const Graph& vecGraph, const bool isReverse)
	{
		std::string postfix = ".vec.graph";
		if (isReverse) postfix = ".vec.rvs.graph";
		const std::string filename = graphName + postfix;
		save_file(filename, vecGraph);
	}

	/// Load graph structure from a file
	static void load_graph_struct(const std::string graphName, Graph& vecGraph, const bool isReverse)
	{
		std::string postfix = ".vec.graph";
		if (isReverse) postfix = ".vec.rvs.graph";
		const std::string filename = graphName + postfix;
		load_file(filename, vecGraph);
	}

	/// Get out-file name
	static std::string get_out_file_name(const std::string graphName, const std::string algName, const int seedsize,
	                                     const std::string probDist, const float probEdge)
	{
		if (probDist == "UNI")
		{
			return graphName + "_" + algName + "_k" + std::to_string(seedsize) + "_" + probDist + std::
				to_string(probEdge);
		}
		return graphName + "_" + algName + "_k" + std::to_string(seedsize) + "_" + probDist;
	}

	/// Print the results
	static void write_result(const std::string outFileName, const PResult resultObj, const std::string outFolder)
	{
		const double runTime = resultObj->get_running_time();
		const double influence = resultObj->get_influence();
		const double influenceOriginal = resultObj->get_influence_original();
		const int seedSize = resultObj->get_seed_size();
		//const int estNodes = resultObj->get_estimated_node_size();

		std::cout << "   --------------------" << std::endl;
		std::cout << "  |Time (sec): " << runTime << std::endl;
		std::cout << "  |Influence: " << influence << std::endl;
		std::cout << "  |Hop-influence: " << influenceOriginal << std::endl;
		std::cout << "  |#Seeds: " << seedSize << std::endl;
		//std::cout << "  |#Estimated-nodes: " << estNodes << std::endl;
		std::cout << "   --------------------" << std::endl;
		CreateDirectoryA(outFolder.c_str(), nullptr);
		std::ofstream outFileNew(outFolder + "/" + outFileName);
		if (outFileNew.is_open())
		{
			outFileNew << "Time (sec): " << runTime << std::endl;
			outFileNew << "Influence: " << influence << std::endl;
			outFileNew << "Hop-influence: " << influenceOriginal << std::endl;
			outFileNew << "#Seeds: " << seedSize << std::endl;
			//outFileNew << "#Estimated-nodes: " << estNodes << std::endl;
			outFileNew.close();
		}
	}

	/// Print the seeds
	static void write_order_seeds(const std::string outFileName, const PResult resultObj, const std::string outFolder)
	{
		auto vecSeed = resultObj->get_seed_vec();
		CreateDirectoryA(outFolder.c_str(), nullptr);
		const auto outpath = outFolder + "/seed";
		CreateDirectoryA(outpath.c_str(), nullptr);
		std::ofstream outFile(outpath + "/seed_" + outFileName);
		for (auto i = 0; i < vecSeed.size(); i++)
		{
			outFile << vecSeed[i] << '\n';
		}
		outFile.close();
	}
};

using TIO = IOcontroller;
using PIO = std::shared_ptr<IOcontroller>;
