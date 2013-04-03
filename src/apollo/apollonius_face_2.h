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
		can_have_e_(!equal(spheres_->at(abc_ids_.get(0)).r, 0) && !equal(spheres_->at(abc_ids_.get(1)).r, 0) && !equal(spheres_->at(abc_ids_.get(2)).r, 0)),
		expansion_for_d_tangent_spheres_(std::max(0.0, std::max(spheres_->at(abc_ids_.get(0)).r, std::max(spheres_->at(abc_ids_.get(1)).r, spheres_->at(abc_ids_.get(2)).r)))*2)
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
		return (
				can_have_d_
				&& d_number<2
				&& (halfspace_of_sphere(tangent_planes_[d_number].first, tangent_planes_[d_number].second, input_sphere)>-1)
				&& !sphere_is_contained_in_recorded_tangent_sphere(d_ids_and_tangent_spheres_, input_sphere, d_number)
				&& !sphere_is_contained_in_recorded_tangent_sphere(e_ids_and_tangent_spheres_, input_sphere, npos)
				);
	}

	std::vector<SimpleSphere> check_candidate_for_d(const std::size_t d_id, const std::size_t d_number) const
	{
		if(
				can_have_d_
				&& d_id!=npos
				&& d_number<2
				&& d_id!=d_ids_and_tangent_spheres_[d_number].first
				&& !abc_ids_.contains(d_id)
				&& !id_equals_recorded_id(e_ids_and_tangent_spheres_, d_id, npos)
				&& (halfspace_of_sphere(tangent_planes_[d_number].first, tangent_planes_[d_number].second, spheres_->at(d_id))>-1)
				&& !sphere_intersects_recorded_tangent_sphere(d_ids_and_tangent_spheres_, spheres_->at(d_id), d_number)
				&& !sphere_intersects_recorded_tangent_sphere(e_ids_and_tangent_spheres_, spheres_->at(d_id), npos)
			)
		{
			const std::vector<SimpleSphere> tangent_spheres=construct_spheres_tangent_sphere<SimpleSphere>(spheres_->at(abc_ids_.get(0)), spheres_->at(abc_ids_.get(1)), spheres_->at(abc_ids_.get(2)), spheres_->at(d_id));
			for(std::size_t i=0;i<tangent_spheres.size();i++)
			{
				const SimpleSphere& tangent_sphere=tangent_spheres[i];
				if(
						!sphere_intersects_recorded_sphere(spheres_, d_ids_and_tangent_spheres_, tangent_sphere, d_number)
						&& !sphere_intersects_recorded_sphere(spheres_, e_ids_and_tangent_spheres_, tangent_sphere, npos)
						&& (tangent_spheres.size()==1 || (halfspace_of_point(tangent_planes_[d_number].first, tangent_planes_[d_number].second, spheres_touching_point<SimpleSphere>(spheres_->at(d_id), tangent_sphere))==1))
					)
				{
					return std::vector<SimpleSphere>(1, tangent_sphere);
				}
			}
		}
		return std::vector<SimpleSphere>();
	}

	void set_d(const std::size_t d_id, const std::size_t d_number, const SimpleSphere& tangent_sphere)
	{
		if(can_have_d_ && d_number<2)
		{
			d_ids_and_tangent_spheres_[d_number]=std::make_pair(d_id, tangent_sphere);
		}
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
				&& (!can_have_d_ || (halfspace_of_sphere(tangent_planes_[0].first, tangent_planes_[0].second, input_sphere)!=1 && halfspace_of_sphere(tangent_planes_[1].first, tangent_planes_[1].second, input_sphere)!=1))
				&& !sphere_is_contained_in_recorded_tangent_sphere(d_ids_and_tangent_spheres_, input_sphere, npos)
				&& !sphere_is_contained_in_recorded_tangent_sphere(e_ids_and_tangent_spheres_, input_sphere, npos)
				&& (!can_have_d_ || d_ids_and_tangent_spheres_[0].first==npos || sphere_intersects_sphere(input_sphere, SimpleSphere(d_ids_and_tangent_spheres_[0].second, d_ids_and_tangent_spheres_[0].second.r+expansion_for_d_tangent_spheres_)))
				&& (!can_have_d_ || d_ids_and_tangent_spheres_[1].first==npos || sphere_intersects_sphere(input_sphere, SimpleSphere(d_ids_and_tangent_spheres_[1].second, d_ids_and_tangent_spheres_[1].second.r+expansion_for_d_tangent_spheres_)))
				);
	}

	std::vector<SimpleSphere> check_candidate_for_e(const std::size_t e_id) const
	{
		if(
				can_have_e_
				&& e_id!=npos
				&& !abc_ids_.contains(e_id)
				&& !id_equals_recorded_id(d_ids_and_tangent_spheres_, e_id, npos)
				&& !id_equals_recorded_id(e_ids_and_tangent_spheres_, e_id, npos)
				&& (!can_have_d_ || (halfspace_of_sphere(tangent_planes_[0].first, tangent_planes_[0].second, spheres_->at(e_id))==-1 && halfspace_of_sphere(tangent_planes_[1].first, tangent_planes_[1].second, spheres_->at(e_id))==-1))
				&& !sphere_intersects_recorded_tangent_sphere(d_ids_and_tangent_spheres_, spheres_->at(e_id), npos)
				&& !sphere_intersects_recorded_tangent_sphere(e_ids_and_tangent_spheres_, spheres_->at(e_id), npos)
				&& (!can_have_d_ || d_ids_and_tangent_spheres_[0].first==npos || sphere_intersects_sphere(spheres_->at(e_id), SimpleSphere(d_ids_and_tangent_spheres_[0].second, d_ids_and_tangent_spheres_[0].second.r+expansion_for_d_tangent_spheres_)))
				&& (!can_have_d_ || d_ids_and_tangent_spheres_[1].first==npos || sphere_intersects_sphere(spheres_->at(e_id), SimpleSphere(d_ids_and_tangent_spheres_[1].second, d_ids_and_tangent_spheres_[1].second.r+expansion_for_d_tangent_spheres_)))
			)
		{
			const std::vector<SimpleSphere> tangent_spheres=construct_spheres_tangent_sphere<SimpleSphere>(spheres_->at(abc_ids_.get(0)), spheres_->at(abc_ids_.get(1)), spheres_->at(abc_ids_.get(2)), spheres_->at(e_id));
			std::vector<SimpleSphere> valid_tangent_spheres;
			for(std::size_t i=0;i<tangent_spheres.size();i++)
			{
				const SimpleSphere& tangent_sphere=tangent_spheres[i];
				if(
						!sphere_intersects_recorded_sphere(spheres_, d_ids_and_tangent_spheres_, tangent_sphere, npos)
						&& !sphere_intersects_recorded_sphere(spheres_, e_ids_and_tangent_spheres_, tangent_sphere, npos)
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
		if(can_have_e_)
		{
			e_ids_and_tangent_spheres_.push_back(std::make_pair(e_id, tangent_sphere));
		}
	}

	void update(const ApolloniusFace2& source)
	{
		if(spheres_==source.spheres_ && abc_ids_==source.abc_ids_)
		{
			for(std::size_t i=0;i<d_ids_and_tangent_spheres_.size() && source.d_ids_and_tangent_spheres_.size();i++)
			{
				if(d_ids_and_tangent_spheres_[i].first==npos && source.d_ids_and_tangent_spheres_[i].first!=npos)
				{
					d_ids_and_tangent_spheres_[i]=source.d_ids_and_tangent_spheres_[i];
				}
			}
			for(std::size_t i=0;source.e_ids_and_tangent_spheres_.size();i++)
			{
				bool found=false;
				for(std::size_t j=0;j<e_ids_and_tangent_spheres_.size() && !found;j++)
				{
					found=(e_ids_and_tangent_spheres_[j].first==source.e_ids_and_tangent_spheres_[i].first
							&& spheres_equal(e_ids_and_tangent_spheres_[j].second, source.e_ids_and_tangent_spheres_[i].second));
				}
				if(!found)
				{
					e_ids_and_tangent_spheres_.push_back(source.e_ids_and_tangent_spheres_[i]);
				}
			}
		}
	}

	std::vector< std::pair<std::size_t, SimpleSphere> > collect_all_recorded_ids_and_tangent_spheres(const bool with_d0, const bool with_d1, const bool with_e) const
	{
		std::vector< std::pair<std::size_t, SimpleSphere> > recorded_ids_and_tangent_spheres;
		recorded_ids_and_tangent_spheres.reserve(d_ids_and_tangent_spheres_.size()+e_ids_and_tangent_spheres_.size());
		for(std::size_t i=0;i<d_ids_and_tangent_spheres_.size();i++)
		{
			if(((i==0 && with_d0) || (i==1 && with_d1)) && d_ids_and_tangent_spheres_[i].first!=npos)
			{
				recorded_ids_and_tangent_spheres.push_back(d_ids_and_tangent_spheres_[i]);
			}
		}
		if(with_e)
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

	std::vector<ApolloniusFace2> produce_faces(const bool with_d0, const bool with_d1, const bool with_e) const
	{
		std::vector<ApolloniusFace2> produced_faces;
		const std::vector< std::pair<std::size_t, SimpleSphere> > recorded_ids_and_tangent_spheres=collect_all_recorded_ids_and_tangent_spheres(with_d0, with_d1, with_e);
		for(int j=0;j<abc_ids_.size();j++)
		{
			for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
			{
				const std::size_t id=recorded_ids_and_tangent_spheres[i].first;
				const SimpleSphere& tangent_sphere=recorded_ids_and_tangent_spheres[i].second;
				ApolloniusFace2 produced_face(*spheres_, Triple(abc_ids_.exclude(j), id));
				if(produced_face.can_have_d_)
				{
					const int h0=halfspace_of_sphere(produced_face.tangent_planes_[0].first, produced_face.tangent_planes_[0].second, produced_face.spheres_->at(id));
					const int h1=halfspace_of_sphere(produced_face.tangent_planes_[1].first, produced_face.tangent_planes_[1].second, produced_face.spheres_->at(id));
					if(h0==-1 && h1==-1)
					{
						produced_face.add_e(id, tangent_sphere);
					}
					else if(h0>-1 && h1==-1)
					{
						produced_face.set_d(id, 0, tangent_sphere);
					}
					else if(h0==-1 && h1>-1)
					{
						produced_face.set_d(id, 1, tangent_sphere);
					}
					else
					{
						if(halfspace_of_point(produced_face.tangent_planes_[0].first, produced_face.tangent_planes_[0].second, spheres_touching_point<SimpleSphere>(produced_face.spheres_->at(id), tangent_sphere))==1)
						{
							produced_face.set_d(id, 0, tangent_sphere);
						}
						else
						{
							produced_face.set_d(id, 1, tangent_sphere);
						}
					}
				}
				else
				{
					produced_face.add_e(id, tangent_sphere);
				}
				bool updated=false;
				for(std::size_t e=0;e<produced_faces.size() && !updated;e++)
				{
					if(produced_faces[e].abc_ids_==produced_face.abc_ids_)
					{
						produced_faces[e].update(produced_face);
						updated=true;
					}
				}
				if(!updated)
				{
					produced_faces.push_back(produced_face);
				}
			}
		}
		return produced_faces;
	}

private:
	static bool id_equals_recorded_id(const std::vector< std::pair<std::size_t, SimpleSphere> >& recorded_ids_and_tangent_spheres, const std::size_t id, const std::size_t excluding_position)
	{
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			if(i!=excluding_position && id==recorded_ids_and_tangent_spheres[i].first)
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	static bool sphere_intersects_recorded_sphere(const std::vector<Sphere>* spheres, const std::vector< std::pair<std::size_t, SimpleSphere> >& recorded_ids_and_tangent_spheres, const InputSphereType& input_sphere, const std::size_t excluding_position)
	{
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			if(i!=excluding_position && recorded_ids_and_tangent_spheres[i].first!=npos && sphere_intersects_sphere(input_sphere, spheres->at(recorded_ids_and_tangent_spheres[i].first)))
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	static bool sphere_intersects_recorded_tangent_sphere(const std::vector< std::pair<std::size_t, SimpleSphere> >& recorded_ids_and_tangent_spheres, const InputSphereType& input_sphere, const std::size_t excluding_position)
	{
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			if(i!=excluding_position && recorded_ids_and_tangent_spheres[i].first!=npos && sphere_intersects_sphere(input_sphere, recorded_ids_and_tangent_spheres[i].second))
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputSphereType>
	static bool sphere_is_contained_in_recorded_tangent_sphere(const std::vector< std::pair<std::size_t, SimpleSphere> >& recorded_ids_and_tangent_spheres, const InputSphereType& input_sphere, const std::size_t excluding_position)
	{
		for(std::size_t i=0;i<recorded_ids_and_tangent_spheres.size();i++)
		{
			if(i!=excluding_position && recorded_ids_and_tangent_spheres[i].first!=npos && sphere_contains_sphere(recorded_ids_and_tangent_spheres[i].second, input_sphere))
			{
				return true;
			}
		}
		return false;
	}

	const std::vector<Sphere>* spheres_;
	Triple abc_ids_;
	std::vector< std::pair<SimplePoint, SimplePoint> > tangent_planes_;
	std::vector< std::pair<std::size_t, SimpleSphere> > d_ids_and_tangent_spheres_;
	std::vector< std::pair<std::size_t, SimpleSphere> > e_ids_and_tangent_spheres_;
	bool can_have_d_;
	bool can_have_e_;
	double expansion_for_d_tangent_spheres_;
};

}

#endif /* APOLLO_APOLLONIUS_FACE_2_H_ */
