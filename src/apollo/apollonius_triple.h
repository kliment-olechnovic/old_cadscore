#ifndef APOLLONIUS_TRIPLE_H_
#define APOLLONIUS_TRIPLE_H_

#include <vector>
#include <map>

#include "spheres_basic_operations.h"
#include "spheres_tangent_plane.h"
#include "spheres_tangent_sphere.h"

namespace apollo
{

class ApolloniusTriple
{
public:
	ApolloniusTriple(const SimpleSphere& a, const SimpleSphere& b, const SimpleSphere& c) :
		spheres_(make_sorted_spheres_vector(a, b, c)),
		tangent_planes_(construct_tangent_planes(a, b, c)),
		bounding_sphere_(construct_bounding_sphere(spheres_)),
		cone_end_(tangent_planes_.empty() ? find_cone_end(spheres_) : 0)
	{
	}

	template<typename InputSphereType>
	bool sphere_may_contain_inner_doubling_sphere(const InputSphereType& x) const
	{
		if(sphere_intersects_sphere(x, bounding_sphere_))
		{
			if(!tangent_planes_.empty())
			{
				for(std::size_t i=0;i<tangent_planes_.size();i++)
				{
					if(halfspace_of_sphere(tangent_planes_[i].first, tangent_planes_[i].second, x)==1)
					{
						return false;
					}
				}
				return true;
			}
			else
			{
				return stick_intersects_sphere(spheres_[0], spheres_[cone_end_], x);
			}
		}
		return false;
	}

	template<typename InputSphereType>
	bool sphere_is_inner_sphere(const InputSphereType& x) const
	{
		if(sphere_contains_sphere(bounding_sphere_, x))
		{
			if(!tangent_planes_.empty())
			{
				for(std::size_t i=0;i<tangent_planes_.size();i++)
				{
					if(halfspace_of_sphere(tangent_planes_[i].first, tangent_planes_[i].second, x)!=-1)
					{
						return false;
					}
				}
				return true;
			}
			else
			{
				return stick_contains_sphere(spheres_[0], spheres_[cone_end_], x);
			}
		}
		return false;
	}

	template<typename OutputSphereType, typename InputSphereType>
	std::vector<OutputSphereType> construct_tangent_spheres(const InputSphereType& x) const
	{
		return construct_spheres_tangent<SimpleSphere>(spheres_[0], spheres_[1], spheres_[2], x);
	}

private:
	std::vector<SimpleSphere> spheres_;
	std::vector< std::pair<SimplePoint, SimplePoint> > tangent_planes_;
	SimpleSphere bounding_sphere_;
	std::size_t cone_end_;

	struct SpheresSortingOperator
	{
		 bool operator()(const SimpleSphere& a, const SimpleSphere& b) const
		 {
			 return (a.r>b.r);
		 }
	};

	static std::vector<SimpleSphere> make_sorted_spheres_vector(const SimpleSphere& a, const SimpleSphere& b, const SimpleSphere& c)
	{
		std::vector<SimpleSphere> spheres;
		spheres.reserve(3);
		spheres.push_back(a);
		spheres.push_back(b);
		spheres.push_back(c);
		std::sort(spheres.begin(), spheres.end(), SpheresSortingOperator());
		return spheres;
	}

	static std::vector< std::pair<SimplePoint, SimplePoint> > construct_tangent_planes(const SimpleSphere& a, const SimpleSphere& b, const SimpleSphere& c)
	{
		const std::vector<SimplePoint> normals=construct_spheres_tangent_plane<SimplePoint>(a, b, c);
		std::vector< std::pair<SimplePoint, SimplePoint> > planes;
		for(std::size_t i=0;i<normals.size();i++)
		{
			planes.push_back(std::make_pair((custom_point_from_object<SimplePoint>(a)+(normals[i]*a.r)), normals[i]));
		}
		return planes;
	}

	static SimpleSphere construct_bounding_sphere(const std::vector<SimpleSphere>& spheres)
	{
		SimplePoint mass_center;
		for(std::size_t i=0;i<spheres.size();i++)
		{
			mass_center=mass_center+custom_point_from_object<SimplePoint>(spheres[i]);
		}
		mass_center=mass_center/3;
		double r=0;
		for(std::size_t i=0;i<spheres.size();i++)
		{
			r=std::max(r, maximal_distance_from_point_to_sphere(mass_center, spheres[i]));
		}
		return SimpleSphere(mass_center.x, mass_center.y, mass_center.z, r);
	}

	static std::size_t find_cone_end(const std::vector<SimpleSphere>& sorted_spheres)
	{
		if(sorted_spheres.size()==3)
		{
			if(stick_contains_sphere(sorted_spheres[0], stick_contains_sphere[1], stick_contains_sphere[2]))
			{
				return 1;
			}
			else if(stick_contains_sphere(sorted_spheres[0], stick_contains_sphere[2], stick_contains_sphere[1]))
			{
				return 2;
			}
		}
		return 0;
	}
};

}

#endif /* APOLLONIUS_TRIPLE_H_ */
