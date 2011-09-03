#ifndef GRAPH_TRAVERSING_H_
#define GRAPH_TRAVERSING_H_

#include <vector>
#include <deque>

class GraphTraverser
{
public:
	typedef bool TraverseType;
	static const bool BFS = true;
	static const bool DFS = false;

	static const size_t npos = -1;

	GraphTraverser(const std::vector< std::vector<std::size_t> >& graph) : graph_(graph), colors_(graph_.size(), -1), current_color_(0), levels_(graph_.size(), 0)
	{
	}

	std::size_t start(const std::size_t id)
	{
		if(id>=graph_.size()) { return npos; }
		current_color_++;
		colors_[id]=current_color_;
		levels_[id]=0;
		queue_.clear();
		queue_.push_back(id);
		return id;
	}

	std::size_t ensure(const std::size_t id)
	{
		if(id>=graph_.size()) { return npos; }
		if(colors_[id]!=current_color_)
		{
			colors_[id]=current_color_;
			levels_[id]=0;
			queue_.push_back(id);
		}
		return id;
	}

	std::size_t next(const TraverseType traverse_type)
	{
		if(queue_.empty())
		{
			return npos;
		}
		else
		{
			const std::size_t current_id=get_current_id(traverse_type);
			const std::vector<std::size_t>& neighbours=graph_[current_id];
			for(std::size_t i=0;i<neighbours.size();i++)
			{
				const std::size_t neighbour_id=neighbours[i];
				if(colors_[neighbour_id]!=current_color_)
				{
					colors_[neighbour_id]=current_color_;
					levels_[neighbour_id]=levels_[current_id]+1;
					queue_.push_back(neighbour_id);
				}
			}
			return current_id;
		}
	}

	std::size_t level(const std::size_t id) const
	{
		return levels_[id];
	}

private:
	std::size_t get_current_id(const TraverseType traverse_type)
	{
		if(traverse_type==BFS)
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

	const std::vector< std::vector<std::size_t> >& graph_;
	std::vector<int> colors_;
	int current_color_;
	std::vector<std::size_t> levels_;
	std::deque<std::size_t> queue_;
};

std::vector<std::size_t> traverse_graph(const std::vector< std::vector<std::size_t> >& graph, const std::size_t start_id, const GraphTraverser::TraverseType traverse_type)
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

std::vector<std::size_t> cluster_graph(const std::vector< std::vector<std::size_t> >& graph, const GraphTraverser::TraverseType traverse_type)
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

#endif /* GRAPH_TRAVERSING_H_ */
