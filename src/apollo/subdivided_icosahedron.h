#ifndef SUBDIVIDED_ICOSAHEDRON_H_
#define SUBDIVIDED_ICOSAHEDRON_H_

#include <cmath>
#include <tr1/unordered_map>

#include "points_basic_operations.h"
#include "tuples.h"

namespace apollo
{

class SubdividedIcosahedron
{
public:
	SubdividedIcosahedron(const std::size_t depth) : center_(0, 0, 0)
	{
		const double t=(1+sqrt(5.0))/2.0;

		vertices_.push_back(SimplePoint( t, 1, 0).unit());
		vertices_.push_back(SimplePoint(-t, 1, 0).unit());
		vertices_.push_back(SimplePoint( t,-1, 0).unit());
		vertices_.push_back(SimplePoint(-t,-1, 0).unit());
		vertices_.push_back(SimplePoint( 1, 0, t).unit());
		vertices_.push_back(SimplePoint( 1, 0,-t).unit());
		vertices_.push_back(SimplePoint(-1, 0, t).unit());
		vertices_.push_back(SimplePoint(-1, 0,-t).unit());
		vertices_.push_back(SimplePoint( 0, t, 1).unit());
		vertices_.push_back(SimplePoint( 0,-t, 1).unit());
		vertices_.push_back(SimplePoint( 0, t,-1).unit());
		vertices_.push_back(SimplePoint( 0,-t,-1).unit());

		triples_.push_back(make_triple(0, 8, 4));
		triples_.push_back(make_triple(1, 10, 7));
		triples_.push_back(make_triple(2, 9, 11));
		triples_.push_back(make_triple(7, 3, 1));
		triples_.push_back(make_triple(0, 5, 10));
		triples_.push_back(make_triple(3, 9, 6));
		triples_.push_back(make_triple(3, 11, 9));
		triples_.push_back(make_triple(8, 6, 4));
		triples_.push_back(make_triple(2, 4, 9));
		triples_.push_back(make_triple(3, 7, 11));
		triples_.push_back(make_triple(4, 2, 0));
		triples_.push_back(make_triple(9, 4, 6));
		triples_.push_back(make_triple(2, 11, 5));
		triples_.push_back(make_triple(0, 10, 8));
		triples_.push_back(make_triple(5, 0, 2));
		triples_.push_back(make_triple(10, 5, 7));
		triples_.push_back(make_triple(1, 6, 8));
		triples_.push_back(make_triple(1, 8, 10));
		triples_.push_back(make_triple(6, 1, 3));
		triples_.push_back(make_triple(11, 7, 5));

		for(std::size_t i=0;i<depth;i++)
		{
			grow();
		}
	}

	const SimplePoint center() const
	{
		return center_;
	}

	const std::vector<SimplePoint>& vertices() const
	{
		return vertices_;
	}

	const std::vector<Triple>& triples() const
	{
		return triples_;
	}

	template<typename SphereType>
	void fit_into_sphere(const SphereType& sphere)
	{
		const SimplePoint new_center=custom_point_from_object<SimplePoint>(sphere);
		for(std::size_t i=0;i<vertices_.size();i++)
		{
			vertices_[i]=new_center+((vertices_[i]-center_).unit()*sphere.r);
		}
		center_=new_center;
	}

	double edge_length_estimate() const
	{
		return distance_from_point_to_point(vertices_.at(triples_.front().get(0)), vertices_.at(triples_.front().get(1)));
	}

private:
	void grow()
	{
		typedef std::tr1::unordered_map<Pair, std::size_t, Pair::HashFunctor> PairsMap;
		PairsMap pairs_vertices;
		std::vector<Triple> new_triples;
		new_triples.reserve(triples_.size()*4);
		std::size_t middle_point_ids[3]={-1,-1,-1};
		for(std::size_t i=0;i<triples_.size();i++)
		{
			const Triple& triple=triples_[i];
			for(int j=0;j<3;j++)
			{
				const Pair pair=triple.exclude(j);
				PairsMap::const_iterator it=pairs_vertices.find(pair);
				if(it==pairs_vertices.end())
				{
					middle_point_ids[j]=vertices_.size();
					vertices_.push_back(((vertices_[pair.get(0)]+vertices_[pair.get(1)])/2).unit());
					pairs_vertices[pair]=middle_point_ids[j];
				}
				else
				{
					middle_point_ids[j]=it->second;
				}
			}
			new_triples.push_back(make_triple(triple.get(0), middle_point_ids[1], middle_point_ids[2]));
			new_triples.push_back(make_triple(triple.get(1), middle_point_ids[0], middle_point_ids[2]));
			new_triples.push_back(make_triple(triple.get(2), middle_point_ids[0], middle_point_ids[1]));
			new_triples.push_back(make_triple(middle_point_ids[0], middle_point_ids[1], middle_point_ids[2]));
		}
		triples_=new_triples;
	}

	SimplePoint center_;
	std::vector<SimplePoint> vertices_;
	std::vector<Triple> triples_;
};

}

#endif /* SUBDIVIDED_ICOSAHEDRON_H_ */
