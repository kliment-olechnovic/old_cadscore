#ifndef SPHERES_H_
#define SPHERES_H_

template<typename PointType>
double distance_from_point_to_point(const PointType& a, const PointType& b)
{
	const double dx=(a.x-b.x);
	const double dy=(a.y-b.y);
	const double dz=(a.z-b.z);
	return sqrt(dx*dx+dy*dy+dz*dz);
}

template<typename SphereType>
double minimal_distance_from_sphere_to_sphere(const SphereType& a, const SphereType& b)
{
	return (distance_from_point_to_point(a, b)-a.r-b.r);
}

template<typename PointType, typename SphereType>
double maximal_distance_from_point_to_sphere(const PointType& a, const SphereType& b)
{
	return (distance_from_point_to_point(a, b)+b.r);
}

template<typename SpheresContainerType>
std::vector< std::vector<std::size_t> > construct_graph_from_spheres_by_distance_threshold(const SpheresContainerType& spheres, const double treshold)
{
	std::vector< std::vector<std::size_t> > graph(spheres.size());
	for(std::size_t i=0;i<spheres.size();i++)
	{
		for(std::size_t j=i+1;j<spheres.size();j++)
		{
			if(minimal_distance_from_sphere_to_sphere(spheres[i], spheres[j])<=treshold)
			{
				graph[i].push_back(j);
				graph[j].push_back(i);
			}
		}
	}
	return graph;
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

#endif /* SPHERES_H_ */
