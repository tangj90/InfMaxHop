#pragma once

/// Node list
typedef std::vector<int> Nodelist;
/// Edge structure, neighbor id and the edge weight
typedef std::pair<int, float> Edge;
/// Edgelist structure from one source/target node
typedef std::vector<Edge> Edgelist;
/// Graph structure
typedef std::vector<Edgelist> Graph;

/// Cascade models: IC, LT
enum CascadeModel { IC, LT };

/// Node element with id and a property value
typedef struct NodeElement
{
	int id;
	double value;
} NodeEleType;

/// Smaller operation for node element
struct smaller
{
	bool operator()(const NodeEleType& Ele1, const NodeEleType& Ele2) const
	{
		return (Ele1.value < Ele2.value);
	}
};

/// Greater operation for node element
struct greater
{
	bool operator()(const NodeEleType& Ele1, const NodeEleType& Ele2) const
	{
		return (Ele1.value > Ele2.value);
	}
};
