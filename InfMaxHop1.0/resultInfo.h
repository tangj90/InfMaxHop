#pragma once

class ResultInfo
{
private:
	double __RunningTime = -1.0, __Influence = -1.0, __InfluenceOriginal = -1.0;
	int __SeedSize = 0, __EstNodeSize = 0;
	std::vector<int> __VecSeed;
public:
	ResultInfo()
	{
	}

	~ResultInfo()
	{
	}

	/// Get running time.
	double get_running_time() const
	{
		return __RunningTime;
	}

	/// Get influence spread.
	double get_influence() const
	{
		return __Influence;
	}

	/// Get hop-based influence spread.
	double get_influence_original() const
	{
		return __InfluenceOriginal;
	}

	/// Get seed sets.
	std::vector<int> get_seed_vec() const
	{
		return __VecSeed;
	}

	/// Get seed size.
	int get_seed_size() const
	{
		return __SeedSize;
	}

	/// Get the number of estimated nodes.
	int get_estimated_node_size() const
	{
		return __EstNodeSize;
	}

	/// Set running time.
	void set_running_time(const double value)
	{
		__RunningTime = value;
	}

	/// Set influence spread.
	void set_influence(const double value)
	{
		__Influence = value;
	}

	/// Set hop-based influence spread
	void set_influence_original(const double value)
	{
		__InfluenceOriginal = value;
	}

	/// Set seed sets.
	void set_seed_vec(std::vector<int>& vecSeed)
	{
		__VecSeed = vecSeed;
		set_seed_size((int)vecSeed.size());
	}

	/// Set seed size.
	void set_seed_size(const int value)
	{
		__SeedSize = value;
	}

	/// Set the number of estimated nodes.
	void set_estimated_node_size(const int value)
	{
		__EstNodeSize = value;
	}

	/// Increase number of estimated nodes.
	void inc_estimated_node_size(const int value)
	{
		__EstNodeSize += value;
	}
};

typedef ResultInfo TResult;
typedef std::shared_ptr<ResultInfo> PResult;
