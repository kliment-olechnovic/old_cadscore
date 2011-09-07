#ifndef APOLLONIUS_GRAPH_H_
#define APOLLONIUS_GRAPH_H_

#include <tr1/unordered_map>

#include "spheres_clustering.h"
#include "spheres_tangent.h"
#include "tuples.h"

namespace apollo
{

template<typename SphereType>
class ApolloniusGraph
{
public:
	typedef SphereType Sphere;
	typedef spheres_clustering<Sphere> clustering;
	typedef typename clustering::Clusters Clusters;

	ApolloniusGraph(const std::vector<Sphere>& spheres, const double clustering_r, const std::size_t clustering_low_count) :
		spheres_(spheres),
		clusters_layers_(clustering::cluster_spheres_until_low_count(spheres, clustering_r, clustering_low_count))
	{
	}

private:
	std::vector<Sphere> spheres_;
	std::vector<Clusters> clusters_layers_;
	std::tr1::unordered_map<Triple, std::size_t, TupleHashOperator<Triple> > triples_;
};

}

#endif /* APOLLONIUS_GRAPH_H_ */
