#ifndef APOLLO2_APOLLONIUS_FACE_2_H_
#define APOLLO2_APOLLONIUS_FACE_2_H_

#include <vector>

#include "tuple.h"
#include "basic_operations_on_points.h"
#include "basic_operations_on_spheres.h"
#include "spheres_tangent_plane.h"
#include "spheres_tangent_sphere.h"

namespace apollo2
{

template<typename SphereType>
class ApolloniusFace
{
public:
	typedef SphereType Sphere;
	static const std::size_t npos=static_cast<std::size_t>(-1);

	ApolloniusFace(const std::vector<Sphere>& spheres, const Triple& abc_ids, const double min_sphere_radius) :
		spheres_(&spheres),
		abc_ids_(abc_ids),
		a_sphere_(&(spheres_->at(abc_ids_.get(0)))),
		b_sphere_(&(spheres_->at(abc_ids_.get(1)))),
		c_sphere_(&(spheres_->at(abc_ids_.get(2)))),
		tangent_planes_(construct_spheres_tangent_planes((*a_sphere_), (*b_sphere_), (*c_sphere_))),
		can_have_d_(tangent_planes_.size()==2),
		can_have_e_(greater(a_sphere_->r, min_sphere_radius) || greater(b_sphere_->r, min_sphere_radius) || greater(c_sphere_->r, min_sphere_radius)),
		threshold_distance_for_e_checking(std::max(0.0, std::max(a_sphere_->r, std::max(b_sphere_->r, c_sphere_->r)))*2-2*min_sphere_radius)
	{
		if(can_have_d_)
		{
			d_ids_and_tangent_spheres_.resize(2, std::pair<std::size_t, SimpleSphere>(npos, SimpleSphere()));
			abc_centers_plane_normal_=plane_normal_from_three_points<SimplePoint>((*a_sphere_), (*b_sphere_), (*c_sphere_));
			if(halfspace_of_point((*a_sphere_), abc_centers_plane_normal_, tangent_planes_[0].first+tangent_planes_[0].second)!=1)
			{
				abc_centers_plane_normal_=abc_centers_plane_normal_.inverted();
			}
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
		return (
				can_have_d_
				&& (d_number<2)
				&& (halfspace_of_sphere(tangent_planes_[d_number].first, tangent_planes_[d_number].second, input_sphere)>=0)
				);
	}

	std::pair<bool, SimpleSphere> check_candidate_for_d(const std::size_t d_id, const std::size_t d_number) const
	{
		if(
				can_have_d_
				&& (d_id!=npos)
				&& (d_number<2)
				&& (d_id!=d_ids_and_tangent_spheres_[d_number].first)
				&& (!abc_ids_.contains(d_id))
				&& (halfspace_of_sphere(tangent_planes_[d_number].first, tangent_planes_[d_number].second, spheres_->at(d_id))>=0)
			)
		{
			const std::vector<SimpleSphere> tangent_spheres=construct_spheres_tangent_sphere<SimpleSphere>((*a_sphere_), (*b_sphere_), (*c_sphere_), spheres_->at(d_id));
			for(std::size_t i=0;i<tangent_spheres.size();i++)
			{
				const SimpleSphere& tangent_sphere=tangent_spheres[i];
				if(
						(!sphere_intersects_recorded_sphere(d_ids_and_tangent_spheres_, tangent_sphere, d_number))
						&& (!sphere_intersects_recorded_sphere(e_ids_and_tangent_spheres_, tangent_sphere, npos))
						&& (tangent_spheres.size()==1 || (halfspace_of_point((*a_sphere_), abc_centers_plane_normal_, tangent_sphere)==(d_number==0 ? 1 : -1)))
					)
				{
					return std::make_pair(true, tangent_sphere);
				}
			}
		}
		return std::make_pair(false, SimpleSphere());
	}

	void set_d(const std::size_t d_id, const std::size_t d_number, const SimpleSphere& tangent_sphere)
	{
		if(can_have_d_ && d_number<2 && d_ids_and_tangent_spheres_[d_number].first!=d_id)
		{
			d_ids_and_tangent_spheres_[d_number]=std::make_pair(d_id, tangent_sphere);
		}
	}

	void set_d_with_d_number_selection(const std::size_t d_id, const SimpleSphere& tangent_sphere)
	{
		if(can_have_d_)
		{
			const int h0=halfspace_of_sphere(tangent_planes_[0].first, tangent_planes_[0].second, spheres_->at(d_id));
			const int h1=halfspace_of_sphere(tangent_planes_[1].first, tangent_planes_[1].second, spheres_->at(d_id));
			if(h0>=0 && h1==-1)
			{
				set_d(d_id, 0, tangent_sphere);
			}
			else if(h0==-1 && h1>=0)
			{
				set_d(d_id, 1, tangent_sphere);
			}
		}
	}

	void unset_d(const std::size_t d_number)
	{
		if(can_have_d_ && d_number<2)
		{
			d_ids_and_tangent_spheres_[d_number]=std::make_pair(npos, SimpleSphere());
		}
	}

	bool has_d(const std::size_t d_number) const
	{
		return (can_have_d_ && d_number<2 && d_ids_and_tangent_spheres_[d_number].first!=npos);
	}

	std::size_t get_d_id(const std::size_t d_number) const
	{
		return ((can_have_d_ && d_number<2) ? d_ids_and_tangent_spheres_[d_number].first : npos);
	}

	const SimpleSphere get_d_tangent_sphere(const std::size_t d_number) const
	{
		return ((can_have_d_ && d_number<2) ? d_ids_and_tangent_spheres_[d_number].second : SimpleSphere());
	}

	bool can_have_e() const
	{
		return can_have_e_;
	}

	template<typename InputSphereType>
	bool sphere_may_contain_candidate_for_e(const InputSphereType& input_sphere) const
	{
		return (
				can_have_e_
				&& (!can_have_d_ || (d_ids_and_tangent_spheres_[0].first==npos || d_ids_and_tangent_spheres_[1].first==npos) || (minimal_distance_from_sphere_to_sphere(input_sphere, d_ids_and_tangent_spheres_[0].second)<=threshold_distance_for_e_checking && minimal_distance_from_sphere_to_sphere(input_sphere, d_ids_and_tangent_spheres_[1].second)<=threshold_distance_for_e_checking))
				&& (!can_have_d_ || (d_ids_and_tangent_spheres_[0].first!=npos && d_ids_and_tangent_spheres_[1].first!=npos) || (halfspace_of_sphere(tangent_planes_[0].first, tangent_planes_[0].second, input_sphere)<=0 && halfspace_of_sphere(tangent_planes_[1].first, tangent_planes_[1].second, input_sphere)<=0))
				);
	}

	std::vector<SimpleSphere> check_candidate_for_e(const std::size_t e_id) const
	{
		if(
				can_have_e_
				&& (e_id!=npos)
				&& (!abc_ids_.contains(e_id))
				&& (!can_have_d_ || (e_id!=d_ids_and_tangent_spheres_[0].first && e_id!=d_ids_and_tangent_spheres_[1].first))
				&& (!can_have_d_ || (d_ids_and_tangent_spheres_[0].first==npos || d_ids_and_tangent_spheres_[1].first==npos) || (minimal_distance_from_sphere_to_sphere(spheres_->at(e_id), d_ids_and_tangent_spheres_[0].second)<=threshold_distance_for_e_checking && minimal_distance_from_sphere_to_sphere(spheres_->at(e_id), d_ids_and_tangent_spheres_[1].second)<=threshold_distance_for_e_checking))
				&& (!can_have_d_ || (halfspace_of_sphere(tangent_planes_[0].first, tangent_planes_[0].second, spheres_->at(e_id))==-1 && halfspace_of_sphere(tangent_planes_[1].first, tangent_planes_[1].second, spheres_->at(e_id))==-1))
			)
		{
			const std::vector<SimpleSphere> tangent_spheres=construct_spheres_tangent_sphere<SimpleSphere>((*a_sphere_), (*b_sphere_), (*c_sphere_), spheres_->at(e_id));
			std::vector<SimpleSphere> valid_tangent_spheres;
			for(std::size_t i=0;i<tangent_spheres.size();i++)
			{
				const SimpleSphere& tangent_sphere=tangent_spheres[i];
				if(
						(!sphere_intersects_recorded_sphere(d_ids_and_tangent_spheres_, tangent_sphere, npos))
						&& (!sphere_intersects_recorded_sphere(e_ids_and_tangent_spheres_, tangent_sphere, npos))
					)
				{
					valid_tangent_spheres.push_back(tangent_sphere);
				}
			}
			return valid_tangent_spheres;
		}
		return std::vector<SimpleSphere>();
	}

	void add_e(const std::size_t e_id, const SimpleSphere& tangent_sphere)
	{
		if(can_have_e_ && e_id!=npos)
		{
			e_ids_and_tangent_spheres_.push_back(std::make_pair(e_id, tangent_sphere));
		}
	}

	bool has_e() const
	{
		return (!e_ids_and_tangent_spheres_.empty());
	}

	std::vector< std::pair<std::size_t, SimpleSphere> > collect_all_recorded_ids_and_tangent_spheres(const bool with_d0, const bool with_d1, const bool with_e) const
	{
		std::vector< std::pair<std::size_t, SimpleSphere> > recorded_ids_and_tangent_spheres;
		recorded_ids_and_tangent_spheres.reserve(d_ids_and_tangent_spheres_.size()+e_ids_and_tangent_spheres_.size());
		if(can_have_d_ && with_d0 && d_ids_and_tangent_spheres_[0].first!=npos)
		{
			recorded_ids_and_tangent_spheres.push_back(d_ids_and_tangent_spheres_[0]);
		}
		if(can_have_d_ && with_d1 && d_ids_and_tangent_spheres_[1].first!=npos)
		{
			recorded_ids_and_tangent_spheres.push_back(d_ids_and_tangent_spheres_[1]);
		}
		if(can_have_e_ && with_e && !e_ids_and_tangent_spheres_.empty())
		{
			recorded_ids_and_tangent_spheres.insert(recorded_ids_and_tangent_spheres.end(), e_ids_and_tangent_spheres_.begin(), e_ids_and_tangent_spheres_.end());
		}
		return recorded_ids_and_tangent_spheres;
	}

	std::vector< std::pair<Quadruple, SimpleSphere> > produce_quadruples(const bool with_d0, const bool with_d1, const bool with_e) const
	{
		std::vector< std::pair<Quadruple, SimpleSphere> > quadruples_with_tangent_spheres;
		const std::vector< std::pair<std::size_t, SimpleSphere> > recorded_ids_and_tangent_spheres=collect_all_recorded_ids_and_tangent_spheres(with_d0, with_d1, with_e);
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			quadruples_with_tangent_spheres.push_back(std::make_pair(Quadruple(abc_ids_, recorded_ids_and_tangent_spheres[i].first), recorded_ids_and_tangent_spheres[i].second));
		}
		return quadruples_with_tangent_spheres;
	}

	std::vector< std::pair<Triple, std::pair<std::size_t, SimpleSphere> > > produce_prefaces(const bool with_d0, const bool with_d1, const bool with_e) const
	{
		std::vector< std::pair<Triple, std::pair<std::size_t, SimpleSphere> > > produced_prefaces;
		const std::vector< std::pair<std::size_t, SimpleSphere> > recorded_ids_and_tangent_spheres=collect_all_recorded_ids_and_tangent_spheres(with_d0, with_d1, with_e);
		for(int j=0;j<3;j++)
		{
			for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
			{
				produced_prefaces.push_back(std::make_pair(Triple(abc_ids_.exclude(j), recorded_ids_and_tangent_spheres[i].first), std::make_pair(abc_ids_.get(j), recorded_ids_and_tangent_spheres[i].second)));
			}
		}
		return produced_prefaces;
	}

private:
	template<typename InputSphereType>
	bool sphere_intersects_recorded_sphere(const std::vector< std::pair<std::size_t, SimpleSphere> >& recorded_ids_and_tangent_spheres, const InputSphereType& input_sphere, const std::size_t excluding_position) const
	{
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			if(i!=excluding_position && recorded_ids_and_tangent_spheres[i].first!=npos && sphere_intersects_sphere(input_sphere, spheres_->at(recorded_ids_and_tangent_spheres[i].first)))
			{
				return true;
			}
		}
		return false;
	}

	const std::vector<Sphere>* spheres_;
	Triple abc_ids_;
	const Sphere* a_sphere_;
	const Sphere* b_sphere_;
	const Sphere* c_sphere_;
	std::vector< std::pair<SimplePoint, SimplePoint> > tangent_planes_;
	std::vector< std::pair<std::size_t, SimpleSphere> > d_ids_and_tangent_spheres_;
	std::vector< std::pair<std::size_t, SimpleSphere> > e_ids_and_tangent_spheres_;
	bool can_have_d_;
	bool can_have_e_;
	double threshold_distance_for_e_checking;
	SimplePoint abc_centers_plane_normal_;
};

}

#endif /* APOLLO2_APOLLONIUS_FACE_2_H_ */
