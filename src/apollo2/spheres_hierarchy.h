#ifndef APOLLO2_SPHERES_HIERARCH_2_H_
#define APOLLO2_SPHERES_HIERARCH_2_H_

#include <vector>
#include <deque>
#include <set>
#include <tr1/functional>
#include <tr1/tuple>

#include "spheres_basic_operations.h"
#include "utilities.h"

namespace apollo2
{

template<typename PrimarySphereType>
class SpheresHierarchy
{
public:
	typedef PrimarySphereType Sphere;

	SpheresHierarchy(const std::vector<Sphere>& spheres, const double r, const std::size_t low_count) :
		spheres_(spheres),
		clusters_layers_(cluster_spheres_until_low_count(spheres_, r, low_count))
	{
	}

	const std::vector<Sphere>& spheres() const
	{
		return spheres_;
	}

	template<typename NodeChecker, typename LeafChecker>
	std::vector<std::size_t> search(
			NodeChecker& node_checker,
			LeafChecker& leaf_checker) const
	{
		typedef std::tr1::tuple<std::size_t, std::size_t, std::size_t> NodeCoordinates;

		std::vector<std::size_t> results;
		if(!clusters_layers_.empty())
		{
			std::deque< NodeCoordinates > stack;
			const std::size_t top_level=clusters_layers_.size()-1;
			for(std::size_t top_id=0;top_id<clusters_layers_[top_level].size();top_id++)
			{
				stack.push_back(NodeCoordinates(top_level, top_id, 0));
			}

			while(!stack.empty())
			{
				const NodeCoordinates current_position=stack.back();
				stack.pop_back();

				const std::size_t current_level=std::tr1::get<0>(current_position);
				const std::size_t current_cluster_id=std::tr1::get<1>(current_position);
				const std::size_t current_child_id=std::tr1::get<2>(current_position);

				if(current_level<clusters_layers_.size()
						&& current_cluster_id<clusters_layers_[current_level].size()
						&& current_child_id<clusters_layers_[current_level][current_cluster_id].children.size())
				{
					const SimpleSphere& sphere=clusters_layers_[current_level][current_cluster_id];
					if(node_checker(sphere))
					{
						const std::vector<std::size_t>& children=clusters_layers_[current_level][current_cluster_id].children;
						if(current_level==0)
						{
							for(std::size_t i=0;i<children.size();i++)
							{
								const std::size_t child=children[i];
								const std::pair<bool, bool> status=leaf_checker(child, spheres_[child]);
								if(status.first)
								{
									results.push_back(child);
									if(status.second)
									{
										return results;
									}
								}
							}
						}
						else
						{
							stack.push_back(NodeCoordinates(current_level, current_cluster_id, current_child_id+1));
							stack.push_back(NodeCoordinates(current_level-1, children[current_child_id], 0));
						}
					}
				}
			}
		}
		return results;
	}

private:
	class Cluster : public SimpleSphere
	{
	public:
		std::vector<std::size_t> children;
	};

	typedef std::vector< std::pair<SimpleSphere, std::vector<std::size_t> > > ClustersLayer;

	template<typename SphereType>
	static std::vector<SimpleSphere> find_clusters_centers(const std::vector<SphereType>& spheres, const double r)
	{
		std::vector<SimpleSphere> centers;
		if(!spheres.empty())
		{
			std::vector<bool> allowed(spheres.size(), true);
			const std::vector<std::size_t> global_traversal=sort_objects_by_functor_result(spheres, std::tr1::bind(maximal_distance_from_point_to_sphere<SphereType, SphereType>, spheres[0], std::tr1::placeholders::_1));
			for(std::size_t k=0;k<spheres.size();k++)
			{
				const std::size_t i=global_traversal[k];
				if(allowed[i])
				{
					const SimpleSphere center=custom_sphere_from_object<SimpleSphere>(spheres[i]);
					centers.push_back(center);
					allowed[i]=false;
					for(std::size_t j=0;j<spheres.size();j++)
					{
						if(maximal_distance_from_point_to_sphere(center, spheres[j])<r*2)
						{
							allowed[j]=false;
						}
					}
				}
			}
		}
		return centers;
	}

	template<typename SphereType>
	static std::vector<Cluster> form_clusters_from_spheres_using_centers(const std::vector<SphereType>& spheres, const std::vector<SimpleSphere>& centers)
	{
		std::vector<Cluster> clusters;
		clusters.reserve(centers.size());
		for(std::size_t i=0;i<centers.size();i++)
		{
			clusters.push_back(custom_sphere_from_object<Cluster>(centers[i]));
		}
		for(std::size_t i=0;i<spheres.size();i++)
		{
			const SphereType& sphere=spheres[i];
			std::size_t min_dist_id=0;
			double min_dist_value=maximal_distance_from_point_to_sphere(clusters[min_dist_id], sphere);
			for(std::size_t j=1;j<clusters.size();j++)
			{
				const double dist_value=maximal_distance_from_point_to_sphere(clusters[j], sphere);
				if(dist_value<min_dist_value)
				{
					min_dist_id=j;
					min_dist_value=dist_value;
				}
			}
			Cluster& cluster=clusters[min_dist_id];
			cluster.r=std::max(cluster.r, min_dist_value);
			cluster.children.push_back(i);
		}
		std::vector<Cluster> nonempty_clusters;
		nonempty_clusters.reserve(clusters.size());
		for(std::size_t i=0;i<clusters.size();i++)
		{
			if(!clusters[i].children.empty())
			{
				nonempty_clusters.push_back(clusters[i]);
			}
		}
		return nonempty_clusters;
	}

	template<typename SphereType>
	static std::vector<Cluster> cluster_spheres(const std::vector<SphereType>& spheres, const double r)
	{
		return form_clusters_from_spheres_using_centers(spheres, find_clusters_centers(spheres, r));
	}

	template<typename SphereType>
	static std::vector< std::vector<Cluster> > cluster_spheres_until_low_count(const std::vector<SphereType>& spheres, const double r, const std::size_t low_count)
	{
		std::vector< std::vector<Cluster> > clusters_layers;
		double using_r=r;
		clusters_layers.push_back(cluster_spheres(spheres, using_r));
		bool need_more=clusters_layers.back().size()>low_count;
		while(need_more)
		{
			using_r*=2;
			const std::vector<Cluster> clusters=cluster_spheres(clusters_layers.back(), using_r);
			if(clusters.size()<clusters_layers.back().size() && clusters.size()>low_count)
			{
				clusters_layers.push_back(clusters);
			}
			else
			{
				need_more=false;
			}
		}
		return clusters_layers;
	}

	const std::vector<Sphere>& spheres_;
	const std::vector< std::vector<Cluster> > clusters_layers_;
};

}

#endif /* APOLLO2_SPHERES_HIERARCH_2_H_ */
