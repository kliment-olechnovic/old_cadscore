#ifndef GRAPH_TRAVERSING_H_
#define GRAPH_TRAVERSING_H_

#include <vector>
#include <deque>

typedef std::vector< std::vector<std::size_t> > Graph;

class GraphTraverser
{
public:
	typedef bool TraverseType;
	static const TraverseType BFS = true;
	static const TraverseType DFS = false;

	static const size_t npos = -1;

	GraphTraverser(const Graph& graph) :
		graph_(graph),
		queued_colors_(graph_.size(), -1),
		returned_colors_(graph_.size(), -1),
		current_color_(0),
		levels_(graph_.size(), 0)
	{
	}

	bool start(const std::size_t id)
	{
		if(id>=graph_.size()) { return false; }

		current_color_++;
		queued_colors_[id]=current_color_;
		levels_[id]=0;
		queue_.clear();
		queue_.push_back(id);
		return true;
	}

	bool force(const std::size_t id, const TraverseType traverse_type)
	{
		if(id>=graph_.size() || returned_colors_[id]==current_color_) { return false; }

		if(queued_colors_[id]!=current_color_)
		{
			queued_colors_[id]=current_color_;
			levels_[id]=0;
		}

		if(traverse_type==BFS)
		{
			queue_.push_front(id);
		}
		else
		{
			queue_.push_back(id);
		}

		return true;
	}

	std::size_t next(const TraverseType traverse_type)
	{
		std::size_t current_id=get_current_id(traverse_type);
		while(current_id!=npos && returned_colors_[current_id]==current_color_)
		{
			current_id=get_current_id(traverse_type);
		}

		if(current_id!=npos)
		{
			const std::vector<std::size_t>& neighbours=graph_[current_id];
			for(std::size_t i=0;i<neighbours.size();i++)
			{
				const std::size_t neighbour_id=neighbours[i];
				if(queued_colors_[neighbour_id]!=current_color_)
				{
					queued_colors_[neighbour_id]=current_color_;
					levels_[neighbour_id]=levels_[current_id]+1;
					queue_.push_back(neighbour_id);
				}
			}
			returned_colors_[current_id]=current_color_;
		}

		return current_id;
	}

	std::size_t level(const std::size_t id) const
	{
		return levels_[id];
	}

private:
	std::size_t get_current_id(const TraverseType traverse_type)
	{
		if(queue_.empty())
		{
			return npos;
		}
		else if(traverse_type==BFS)
		{
			const std::size_t current_id=queue_.front();
			queue_.pop_front();
			return current_id;
		}
		else
		{
			const std::size_t current_id=queue_.back();
			queue_.pop_back();
			return current_id;
		}
	}

	const Graph& graph_;
	std::vector<int> queued_colors_;
	std::vector<int> returned_colors_;
	int current_color_;
	std::vector<std::size_t> levels_;
	std::deque<std::size_t> queue_;
};

std::vector<std::size_t> traverse_graph(const Graph& graph, const std::size_t start_id, const GraphTraverser::TraverseType traverse_type)
{
	GraphTraverser gt(graph);
	std::vector<std::size_t> result;
	result.reserve(graph.size());
	gt.start(start_id);
	std::size_t id=gt.next(true);
	while(id!=gt.npos)
	{
		result.push_back(id);
		id=gt.next(true);
	}
	return result;
}

std::vector<std::size_t> collect_connected_components_from_graph(const Graph& graph, const GraphTraverser::TraverseType traverse_type)
{
	GraphTraverser gt(graph);
	std::vector<std::size_t> clusters;
	std::vector<std::size_t> clusters_map(graph.size(), gt.npos);
	for(std::size_t i=0;i<clusters_map.size();i++)
	{
		if(clusters_map[i]==gt.npos)
		{
			gt.start(i);
			std::size_t id=gt.next(traverse_type);
			while(id!=gt.npos)
			{
				clusters_map[id]=i;
				id=gt.next(traverse_type);
			}
			clusters.push_back(i);
		}
	}
	return clusters;
}

std::vector<std::size_t> subdivide_graph(const Graph& graph, const std::size_t depth)
{
	std::vector<std::size_t> buckets;
	std::vector<std::size_t> permutation=traverse_graph(graph, 0, GraphTraverser::BFS);
	GraphTraverser gt(graph);
	std::vector<bool> labels(graph.size(), true);
	for(std::size_t j=0;j<graph.size();j++)
	{
		const std::size_t i=permutation[j];
		if(labels[i]==true)
		{
			gt.start(i);
			std::size_t id=gt.next(gt.BFS);
			while(id!=gt.npos && gt.level(id)<=depth)
			{
				labels[id]=false;
				id=gt.next(gt.BFS);
			}
			buckets.push_back(i);
		}
	}
	return buckets;
}

#endif /* GRAPH_TRAVERSING_H_ */
