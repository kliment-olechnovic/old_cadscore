#ifndef SURFING_H_
#define SURFING_H_

#include <vector>

class Traverser
{
public:
	static const size_t npos = -1;

	Traverser(const std::vector< std::vector<std::size_t> >& graph) : graph_(graph), colors_(grap_.size(), -1), current_color_(0)
	{
	}

	std::size_t start(const std::size_t id)
	{
		if(id>=graph_.size()) { return npos; }
		current_color_++;
		colors_[id]=current_color_;
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
			queue_.push_back(id);
		}
		return id;
	}

	std::size_t bfs_next()
	{
		return next(true);
	}

	std::size_t dfs_next()
	{
		return next(false);
	}

	std::vector<std::size_t> cluster()
	{
		std::vector<std::size_t> clusters;
		std::vector<std::size_t> clusters_map(graph_.size(), npos);
		for(std::size_t i=0;i<clusters_map;i++)
		{
			if(clusters_map[i]==npos)
			{
				start(i);
				{
					std::size_t next_id=dfs_next();
					while(next_id!=npos)
					{
						clusters_map[next_id]=i;
						next_id=dfs_next();
					}
				}
				cluster.push_back(i);
			}
		}
		return clusters;
	}

private:
	const std::vector< std::vector<std::size_t> >& graph_;
	std::vector<int> colors_;
	int current_color_;
	std::list<std::size_t> queue_;

	std::size_t bfs_current_id()
	{
		const std::size_t current_id=queue_.front();
		queue_.pop_front();
		return current_id;
	}

	std::size_t dfs_current_id()
	{
		const std::size_t current_id=queue_.back();
		queue_.pop_back();
		return current_id;
	}

	std::size_t next(bool bfs)
	{
		if(queue_.empty())
		{
			return npos;
		}
		else
		{
			const std::size_t current_id=(bfs ? bfs_current_id() : dfs_current_id());
			const std::vector<std::size_t>& neighbours=graph_[current_id];
			for(std::size_t i=0;i<neighbours.size();i++)
			{
				const std::size_t neighbour_id=neighbours[i];
				if(colors_[neighbour_id]!=current_color_)
				{
					colors_[neighbour_id]=current_color_;
					queue_.push_back(neighbour_id);
				}
			}
			return current_id;
		}
	}
};

#endif /* SURFING_H_ */
