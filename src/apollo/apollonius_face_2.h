#ifndef APOLLO_APOLLONIUS_FACE_2_H_
#define APOLLO_APOLLONIUS_FACE_2_H_

#include <vector>

#include "tuples.h"
#include "points_basic_operations.h"
#include "spheres_basic_operations.h"
#include "spheres_tangent_plane.h"
#include "spheres_tangent_sphere.h"

namespace apollo
{

template<typename SphereType>
class ApolloniusFace2
{
public:
	typedef SphereType Sphere;
	static const std::size_t npos=static_cast<std::size_t>(-1);

	ApolloniusFace2(const std::vector<Sphere>& spheres, const Triple& abc_ids) :
		spheres_(&spheres),
		abc_ids_(abc_ids),
		tangent_planes_(construct_spheres_tangent_planes(spheres_->at(abc_ids_.get(0)), spheres_->at(abc_ids_.get(1)), spheres_->at(abc_ids_.get(2)))),
		can_have_d_(tangent_planes_.size()==2),
		can_have_e_(!equal(spheres_->at(abc_ids_.get(0)).r, 0) && !equal(spheres_->at(abc_ids_.get(1)).r, 0) && !equal(spheres_->at(abc_ids_.get(2)).r, 0))
	{
		if(can_have_d_)
		{
			d_ids_and_tangent_spheres_.resize(2, std::pair<std::size_t, SimpleSphere>(npos, SimpleSphere()));
		}
		else
		{
			tangent_planes_.clear();
		}
	}

	const Triple& abc_ids() const
	{
		return abc_ids_;
	}

	bool can_have_d() const
	{
		return can_have_d_;
	}

	template<typename InputSphereType>
	bool sphere_may_contain_candidate_for_d(const InputSphereType& input_sphere, const std::size_t d_number) const
	{
		return (can_have_d_
				&& d_number<2
				&& (halfspace_of_sphere(tangent_planes_[d_number].first, tangent_planes_[d_number].second, input_sphere)>-1));
	}

	std::pair<bool, SimpleSphere> check_candidate_for_d(const std::size_t d_id, const std::size_t d_number) const
	{
		if(can_have_d_
				&& d_id!=npos
				&& d_id!=d_ids_and_tangent_spheres_[d_number].first
				&& d_number<2
				&& !abc_ids_.contains(d_id)
				&& !id_equals_recorded_e_id(d_id))
		{
			const std::size_t other_d_number=(d_number==0 ? 1 : 0);
			const std::size_t other_d_id=d_ids_and_tangent_spheres_[other_d_number].first;
			const Sphere& d_sphere=spheres_->at(d_id);
			if((other_d_id==npos || !sphere_intersects_sphere(d_sphere, d_ids_and_tangent_spheres_[other_d_number].second))
					&& sphere_may_contain_candidate_for_d(d_sphere, d_number)
					&& !sphere_intersects_recorded_e_tangent_spheres(e_sphere))
			{
				const std::vector<SimpleSphere> tangent_spheres=construct_spheres_tangent_sphere<SimpleSphere>(spheres_->at(abc_ids_.get(0)), spheres_->at(abc_ids_.get(1)), spheres_->at(abc_ids_.get(2)), d_sphere);
				for(std::size_t i=0;i<tangent_spheres.size();i++)
				{
					const SimpleSphere& tangent_sphere=tangent_spheres[i];
					if((other_d_id==npos || !sphere_intersects_sphere(tangent_sphere, spheres_->at(other_d_id)))
							&& !sphere_intersects_recorded_e(tangent_sphere))
					{
						if(tangent_spheres.size()==1
								|| (halfspace_of_point(tangent_planes_[d_number].first, tangent_planes_[d_number].second, SimplePoint(d_sphere)+((SimplePoint(tangent_sphere)-SimplePoint(d_sphere)).unit()*d_sphere.r))==1))
						{
							return std::make_pair(true, tangent_sphere);
						}
					}
				}
			}
		}
		return std::make_pair(false, SimpleSphere());
	}

	bool can_have_e() const
	{
		return can_have_e_;
	}

	template<typename InputSphereType>
	bool sphere_may_contain_candidate_for_e(const InputSphereType& input_sphere) const
	{
		if(!can_have_e_
				|| sphere_is_contained_in_recorded_d_tangent_spheres(input_sphere)
				|| sphere_is_contained_in_recorded_e_tangent_spheres(input_sphere))
		{
			return false;
		}
		if(can_have_d_)
		{
			for(std::size_t i=0;i<tangent_planes_.size();i++)
			{
				if(halfspace_of_sphere(tangent_planes_[i].first, tangent_planes_[i].second, input_sphere)==1)
				{
					return false;
				}
			}
			double expansion_radius=0;
			for(int i=0;i<abc_ids_.size();i++)
			{
				expansion_radius=std::max(expansion_radius, spheres_->at(abc_ids_.get(i)).r*2);
			}
			for(std::size_t i=0;i<d_ids_and_tangent_spheres_.size();i++)
			{
				if(d_ids_and_tangent_spheres_[i].first!=npos)
				{
					if(!sphere_intersects_sphere(input_sphere, SimpleSphere(d_ids_and_tangent_spheres_[i].second, d_ids_and_tangent_spheres_[i].r+expansion_radius)))
					{
						return false;
					}
				}
			}
		}
		return true;
	}

	std::vector<SimpleSphere> check_candidate_for_e(const std::size_t e_id) const
	{
		if(can_have_e_
				&& e_id!=npos
				&& !abc_ids_.contains(e_id)
				&& !id_equals_recorded_d_id(e_id)
				&& !id_equals_recorded_e_id(e_id))
		{
			const Sphere& e_sphere=spheres_->at(e_id);
			if((!can_have_d_ || (halfspace_of_sphere(tangent_planes_[0].first, tangent_planes_[0].second, e_sphere)==-1 && halfspace_of_sphere(tangent_planes_[1].first, tangent_planes_[1].second, e_sphere)==-1))
					&& sphere_may_contain_candidate_for_e(e_sphere)
					&& !sphere_intersects_recorded_d_tangent_spheres(e_sphere)
					&& !sphere_intersects_recorded_e_tangent_spheres(e_sphere))
			{
				const std::vector<SimpleSphere> tangent_spheres=construct_spheres_tangent_sphere<SimpleSphere>(spheres_->at(abc_ids_.get(0)), spheres_->at(abc_ids_.get(1)), spheres_->at(abc_ids_.get(2)), e_sphere);
				std::vector<SimpleSphere> valid_tangent_spheres;
				for(std::size_t i=0;i<tangent_spheres.size();i++)
				{
					const SimpleSphere& tangent_sphere=tangent_spheres[i];
					if(!sphere_intersects_recorded_d(tangent_sphere)
							&& !sphere_intersects_recorded_e(tangent_sphere))
					{
						valid_tangent_spheres.push_back(tangent_sphere);
					}
				}
				return valid_tangent_spheres;
			}
		}
		return std::vector<SimpleSphere>();
	}

private:
	bool id_equals_recorded_d_id(const std::size_t id) const
	{
		for(std::size_t i=0;i<d_ids_and_tangent_spheres_.size();i++)
		{
			if(id==d_ids_and_tangent_spheres_[i].first)
			{
				return true;
			}
		}
		return false;
	}

	bool id_equals_recorded_e_id(const std::size_t id) const
	{
		for(std::size_t i=0;i<e_ids_and_tangent_spheres_.size();i++)
		{
			if(id==e_ids_and_tangent_spheres_[i].first)
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	bool sphere_intersects_recorded_d(const InputSphereType& input_sphere) const
	{
		for(std::size_t i=0;i<d_ids_and_tangent_spheres_.size();i++)
		{
			if(d_ids_and_tangent_spheres_[i].first!=npos && sphere_intersects_sphere(input_sphere, spheres_->at(d_ids_and_tangent_spheres_[i].first)))
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	bool sphere_intersects_recorded_e(const InputSphereType& input_sphere) const
	{
		for(std::size_t i=0;i<e_ids_and_tangent_spheres_.size();i++)
		{
			if(sphere_intersects_sphere(input_sphere, spheres_->at(e_ids_and_tangent_spheres_[i].first)))
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	bool sphere_intersects_recorded_d_tangent_spheres(const InputSphereType& input_sphere) const
	{
		for(std::size_t i=0;i<d_ids_and_tangent_spheres_.size();i++)
		{
			if(d_ids_and_tangent_spheres_[i].first!=npos && sphere_intersects_sphere(input_sphere, d_ids_and_tangent_spheres_[i].second))
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	bool sphere_intersects_recorded_e_tangent_spheres(const InputSphereType& input_sphere) const
	{
		for(std::size_t i=0;i<e_ids_and_tangent_spheres_.size();i++)
		{
			for(std::size_t j=0;j<e_ids_and_tangent_spheres_[i].second.size();j++)
			{
				if(sphere_intersects_sphere(input_sphere, e_ids_and_tangent_spheres_[i].second[j]))
				{
					return true;
				}
			}
		}
		return false;
	}

	template<typename InputSphereType>
	bool sphere_is_contained_in_recorded_d_tangent_spheres(const InputSphereType& input_sphere) const
	{
		for(std::size_t i=0;i<d_ids_and_tangent_spheres_.size();i++)
		{
			if(d_ids_and_tangent_spheres_[i].first!=npos && sphere_contains_sphere(d_ids_and_tangent_spheres_[i].second, input_sphere))
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	bool sphere_is_contained_in_recorded_e_tangent_spheres(const InputSphereType& input_sphere) const
	{
		for(std::size_t i=0;i<e_ids_and_tangent_spheres_.size();i++)
		{
			for(std::size_t j=0;j<e_ids_and_tangent_spheres_[i].second.size();j++)
			{
				if(sphere_contains_sphere(e_ids_and_tangent_spheres_[i].second[j], input_sphere))
				{
					return true;
				}
			}
		}
		return false;
	}

	const std::vector<Sphere>* spheres_;
	Triple abc_ids_;
	std::vector< std::pair<SimplePoint, SimplePoint> > tangent_planes_;
	std::vector< std::pair<std::size_t, SimpleSphere> > d_ids_and_tangent_spheres_;
	std::vector< std::pair<std::size_t, std::vector<SimpleSphere> > > e_ids_and_tangent_spheres_;
	bool can_have_d_;
	bool can_have_e_;
};

}

#endif /* APOLLO_APOLLONIUS_FACE_2_H_ */
