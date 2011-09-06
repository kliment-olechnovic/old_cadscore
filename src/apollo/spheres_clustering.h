#ifndef SPHERES_CLUSTERING_H_
#define SPHERES_CLUSTERING_H_

#include <vector>

#include "distances.h"

namespace apollo
{

template<typename SphereType>
std::vector<SphereType> find_clusters_centers(const std::vector<SphereType>& spheres, const double r)
{
	std::vector<SphereType> centers;
	std::vector<bool> allowed(spheres.size(), true);
	const std::vector<std::size_t> global_traversal=sort_objects_by_distances(spheres[0], spheres, std::ptr_fun(maximal_distance_from_point_to_sphere<SphereType, SphereType>));
	for(std::size_t k=0;k<spheres.size();k++)
	{
		const std::size_t i=global_traversal[k];
		if(allowed[i])
		{
			const SphereType& center=spheres[i];
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

template<typename SphereType>
std::vector< std::pair<SphereType, std::vector<std::size_t> > > form_clusters_from_spheres_using_centers(const std::vector<SphereType>& spheres, const std::vector<SphereType>& centers)
{
	std::vector< std::pair<SphereType, std::vector<std::size_t> > > clusters;
	for(std::size_t i=0;i<centers.size();i++)
	{
		clusters.push_back(std::make_pair(centers[i], std::vector<std::size_t>()));
	}

	for(std::size_t i=0;i<spheres.size();i++)
	{
		const SphereType& sphere=spheres[i];
		std::size_t min_dist_id=0;
		for(std::size_t j=1;j<clusters.size();j++)
		{
			if( maximal_distance_from_point_to_sphere(clusters[j].first, sphere) <
					maximal_distance_from_point_to_sphere(clusters[min_dist_id].first, sphere))
			{
				min_dist_id=j;
			}
		}
		std::pair<SphereType, std::vector<std::size_t> >& cluster=clusters[min_dist_id];
		cluster.first.r=std::max(cluster.first.r, maximal_distance_from_point_to_sphere(cluster.first, sphere));
		cluster.second.push_back(i);
	}

	std::vector< std::pair<SphereType, std::vector<std::size_t> > > nonempty_clusters;
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

template<typename SphereType>
std::vector< std::pair<SphereType, std::vector<std::size_t> > > cluster_spheres(const std::vector<SphereType>& spheres, const double r)
{
	return form_clusters_from_spheres_using_centers(spheres, find_clusters_centers(spheres, r));
}

}

#endif /* SPHERES_CLUSTERING_H_ */
