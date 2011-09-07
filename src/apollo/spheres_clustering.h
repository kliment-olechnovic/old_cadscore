#ifndef SPHERES_CLUSTERING_H_
#define SPHERES_CLUSTERING_H_

#include <vector>

#include "distances.h"
#include "containers_utilities.h"

namespace apollo
{

template<typename SphereType>
class spheres_clustering
{
public:
	typedef SphereType Sphere;
	typedef std::vector< std::pair<Sphere, std::vector<std::size_t> > > Clusters;

	static std::vector<Sphere> find_clusters_centers(const std::vector<Sphere>& spheres, const double r)
	{
		std::vector<Sphere> centers;
		std::vector<bool> allowed(spheres.size(), true);
		const std::vector<std::size_t> global_traversal=sort_objects_by_distances(spheres[0], spheres, std::ptr_fun(maximal_distance_from_point_to_sphere<Sphere, Sphere>));
		for(std::size_t k=0;k<spheres.size();k++)
		{
			const std::size_t i=global_traversal[k];
			if(allowed[i])
			{
				const Sphere& center=spheres[i];
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
		return centers;
	}

	static Clusters form_clusters_from_spheres_using_centers(const std::vector<Sphere>& spheres, const std::vector<Sphere>& centers)
	{
		Clusters clusters;
		for(std::size_t i=0;i<centers.size();i++)
		{
			clusters.push_back(std::make_pair(centers[i], std::vector<std::size_t>()));
		}

		for(std::size_t i=0;i<spheres.size();i++)
		{
			const Sphere& sphere=spheres[i];
			std::size_t min_dist_id=0;
			for(std::size_t j=1;j<clusters.size();j++)
			{
				if( maximal_distance_from_point_to_sphere(clusters[j].first, sphere) <
						maximal_distance_from_point_to_sphere(clusters[min_dist_id].first, sphere))
				{
					min_dist_id=j;
				}
			}
			std::pair<Sphere, std::vector<std::size_t> >& cluster=clusters[min_dist_id];
			cluster.first.r=std::max(cluster.first.r, maximal_distance_from_point_to_sphere(cluster.first, sphere));
			cluster.second.push_back(i);
		}

		std::vector< std::pair<Sphere, std::vector<std::size_t> > > nonempty_clusters;
		nonempty_clusters.reserve(clusters.size());
		for(std::size_t i=0;i<clusters.size();i++)
		{
			if(!clusters[i].second.empty())
			{
				nonempty_clusters.push_back(clusters[i]);
			}
		}

		return nonempty_clusters;
	}

	static Clusters cluster_spheres(const std::vector<Sphere>& spheres, const double r)
	{
		return form_clusters_from_spheres_using_centers(spheres, find_clusters_centers(spheres, r));
	}

	static std::vector<Clusters> cluster_spheres_until_low_count(const std::vector<Sphere>& spheres, const double r, const std::size_t low_count)
	{
		std::vector<Clusters> clusters_layers;
		double using_r=r;
		clusters_layers.push_back(cluster_spheres(spheres, using_r));
		bool need_more=clusters_layers.back().size()>low_count;
		while(need_more)
		{
			using_r*=2;
			const Clusters clusters=cluster_spheres(split_pairs(clusters_layers.back()).first, using_r);
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
};

}

#endif /* SPHERES_CLUSTERING_H_ */
