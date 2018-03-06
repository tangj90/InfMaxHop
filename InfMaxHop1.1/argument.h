#pragma once


class Argument
{
public:
	int _func = 1; // Function parameter. 0: format graph, 1: maximize profit, 2: format graph and then maximize profit.
	int _seedsize = 50; // The number of nodes to be selected. Default is 50.
	float _probEdge = float(0.1); // For the UNI setting, every edge has the same diffusion probability.
	std::string _graphname = "facebook"; // Graph name. Default is "facebook".
	std::string _mode = "g"; // g: graph only (default, using WC), w: with edge property,
	std::string _dir = "graphInfo"; // Directory
	std::string _outFileName; // File name of the result
	std::string _resultFolder = "result"; // Result folder. Default is "test".
	std::string _algName = "oneHop"; // Algorithm. Default is oneHop.
	std::string _probDist = "WC"; // Probability distribution for diffusion model. Option: WC, TR, UNI. Default is WC.
	CascadeModel _model = IC; // Cascade models: IC, LT. Default is IC.
	bool _opt = true; // Whether the optimization via upper bounds is used. Default is true. 

	Argument(int argc, char* argv[])
	{
		std::string param, value;
		for (int ind = 1; ind < argc; ind++)
		{
			if (argv[ind][0] != '-') break;
			std::stringstream sstr(argv[ind]);
			getline(sstr, param, '=');
			getline(sstr, value, '=');
			if (!param.compare("-func")) _func = stoi(value);
			else if (!param.compare("-seedsize")) _seedsize = stoi(value);
			else if (!param.compare("-pedge")) _probEdge = stof(value);
			else if (!param.compare("-gname")) _graphname = value;
			else if (!param.compare("-mode")) _mode = value;
			else if (!param.compare("-dir")) _dir = value;
			else if (!param.compare("-outpath")) _resultFolder = value;
			else if (!param.compare("-alg")) _algName = value;
			else if (!param.compare("-pdist")) _probDist = value;
			else if (!param.compare("-model")) _model = value == "LT" ? LT : IC;
			else if (!param.compare("-opt")) _opt = stoi(value) > 0;
		}
		_outFileName = TIO::get_out_file_name(_graphname, _algName, _seedsize, _probDist, _probEdge);
		if (tolower(_algName[0]) == 't' && !_opt)
		{
			_outFileName = TIO::get_out_file_name(_graphname, _algName + "_nonopt", _seedsize, _probDist, _probEdge);
		}
		if (_model == LT) _outFileName = "LT_" + _outFileName;
	}

	std::string get_outfilename_with_alg(const std::string& algName) const
	{
		return TIO::get_out_file_name(_graphname, algName, _seedsize, _probDist, _probEdge);
	}
};

using TArgument = Argument;
using PArgument = std::shared_ptr<TArgument>;
