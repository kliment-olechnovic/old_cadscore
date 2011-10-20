#ifndef APOLLONIUS_FACE_H_
#define APOLLONIUS_FACE_H_

#include <vector>
#include <map>
#include <stdexcept>
#include <iostream>

#include "tuples.h"
#include "points_basic_operations.h"
#include "spheres_basic_operations.h"
#include "spheres_tangent_plane.h"
#include "spheres_tangent_sphere.h"
#include "spheres_tangent_stick.h"

namespace apollo
{

template<typename SphereType>
class ApolloniusFace
{
public:
	typedef SphereType Sphere;
	typedef std::map< std::size_t, std::vector<SimpleSphere> > ContainerForD3;
	static const std::size_t npos=-1;

	ApolloniusFace(
			const std::vector<Sphere>& spheres,
			const Triple& abc_ids,
			const std::size_t d1_id,
			const SimpleSphere& d1_tangent_sphere) :
				spheres_(&spheres),
				abc_ids_(abc_ids),
				a_(&(spheres_->at(abc_ids_.get(0)))),
				b_(&(spheres_->at(abc_ids_.get(1)))),
				c_(&(spheres_->at(abc_ids_.get(2)))),
				d1_id_(d1_id),
				d1_(&(spheres_->at(d1_id_))),
				d1_tangent_sphere_(d1_tangent_sphere),
				tangent_planes_(construct_spheres_tangent_planes(*a_, *b_, *c_)),
				tangent_stick_(tangent_planes_.empty() ? select_tangent_stick(*a_, *b_, *c_) : std::pair<const Sphere*, const Sphere*>(NULL, NULL)),
				has_valid_tangency_information_(tangent_planes_.size()==2 || (tangent_stick_.first!=NULL && tangent_stick_.second!=NULL)),
				can_have_d2_(has_valid_tangency_information_),
				free_tangent_plane_id_(select_free_tangent_plane_id(*a_, *b_, *c_, tangent_planes_, *d1_, d1_tangent_sphere_)),
				centroid_(calculate_centroid(*a_, *b_, *c_)),
				can_have_d3_(has_valid_tangency_information_ && (!equal(a_->r, 0) || !equal(b_->r, 0) || !equal(c_->r, 0))),
				d2_id_(npos),
				d2_tangent_sphere_(SimpleSphere())
	{
		if(abc_ids_.has_repetetions())
		{
			throw std::logic_error("Invalid abc IDs");
		}

		if(abc_ids_.contains(d1_id_))
		{
			throw std::logic_error("Invalid d1 ID");
		}

		if(!check_spheres_tangent(*a_, *b_, *c_, *d1_, d1_tangent_sphere_))
		{
			throw std::logic_error("Invalid d1 tangent sphere");
		}

		if(!has_valid_tangency_information_)
		{
			std::clog << "Failed to aquire valid tangency information for the following 3 spheres:\n";
			std::clog << "SPHERE " << a_->r << " " << a_->x << " " << a_->y << " " << a_->z << "\n";
			std::clog << "SPHERE " << b_->r << " " << b_->x << " " << b_->y << " " << b_->z << "\n";
			std::clog << "SPHERE " << c_->r << " " << c_->x << " " << c_->y << " " << c_->z << "\n";
		}
	}

	const Triple& abc_ids() const
	{
		return abc_ids_;
	}

	std::size_t d1_id() const
	{
		return d1_id_;
	}

	const SimpleSphere& d1_tangent_sphere() const
	{
		return d1_tangent_sphere_;
	}

	const bool can_have_d2() const
	{
		return can_have_d2_;
	}

	std::size_t d2_id() const
	{
		return d2_id_;
	}

	const SimpleSphere& d2_tangent_sphere() const
	{
		return d2_tangent_sphere_;
	}

	bool can_have_d3() const
	{
		return can_have_d3_;
	}

	const ContainerForD3& d3_ids_and_tangent_spheres() const
	{
		return d3_ids_and_tangent_spheres_;
	}

	const Quadruple quadruple_with_d1() const
	{
		return Quadruple(abc_ids_, d1_id_);
	}

	template<typename InputSphereType>
	bool sphere_may_contain_candidate_for_d2(const InputSphereType& x) const
	{
		return (can_have_d2_ && (free_tangent_plane_id_!=npos ? sphere_is_on_free_plane(x) : !sphere_is_inner(x)));
	}

	std::pair<bool, SimpleSphere> check_candidate_for_d2(const std::size_t d2_id) const
	{
		if(can_have_d2_ && d2_id!=npos && !abc_ids_.contains(d2_id))
		{
			const Sphere& d2=spheres_->at(d2_id);
			if(sphere_may_contain_candidate_for_d2(d2) && !sphere_intersects_sphere(d1_tangent_sphere_, d2))
			{
				const std::vector<SimpleSphere> tangents=construct_spheres_tangent<SimpleSphere>(*a_, *b_, *c_, d2);
				for(std::size_t i=0;i<tangents.size();i++)
				{
					if(!sphere_intersects_sphere(tangents[i], *d1_))
					{
						if(d2_id==d1_id_)
						{
							if(!spheres_equal(tangents[i], d1_tangent_sphere_))
							{
								return std::make_pair(true, tangents[i]);
							}
						}
						else
						{
							return std::make_pair(true, tangents[i]);
						}
					}
				}
			}
		}
		return std::make_pair(false, SimpleSphere());
	}

	template<typename InputSphereType>
	bool sphere_may_contain_candidate_for_d3(const InputSphereType& x) const
	{
		return (can_have_d3_ && sphere_may_contain_inner_sphere(x) && !sphere_is_outside_the_bounding_sphere_of_d1_and_d2(x));
	}

	std::pair<bool, std::vector<SimpleSphere> > check_candidate_for_d3(const std::size_t d3_id) const
	{
		if(can_have_d3_ && d3_id!=npos && d3_id!=d1_id_ && d3_id!=d2_id_ && !abc_ids_.contains(d3_id))
		{
			const Sphere& d3=spheres_->at(d3_id);
			if(sphere_is_inner(d3)
					&& !sphere_is_outside_the_bounding_sphere_of_d1_and_d2(d3)
					&& !sphere_intersects_sphere(d1_tangent_sphere_, d3)
					&& (d2_id_==npos || !sphere_intersects_sphere(d2_tangent_sphere_, d3)))
			{

				bool no_conflict_with_other_d3_tangents=true;
				for(ContainerForD3::const_iterator it=d3_ids_and_tangent_spheres_.begin();it!=d3_ids_and_tangent_spheres_.end() && no_conflict_with_other_d3_tangents;it++)
				{
					for(std::size_t i=0;i<it->second.size();i++)
					{
						no_conflict_with_other_d3_tangents=no_conflict_with_other_d3_tangents && !sphere_intersects_sphere(it->second[i], d3);
					}
				}
				if(no_conflict_with_other_d3_tangents)
				{
					const std::vector<SimpleSphere> tangents=construct_spheres_tangent<SimpleSphere>(*a_, *b_, *c_, d3);
					std::vector<SimpleSphere> valid_tangents;
					for(std::size_t i=0;i<tangents.size();i++)
					{
						if(!sphere_intersects_sphere(tangents[i], *d1_) && (d2_id_==npos || !sphere_intersects_sphere(tangents[i], spheres_->at(d2_id_))))
						{
							bool no_conflict_with_other_d3=true;
							for(ContainerForD3::const_iterator it=d3_ids_and_tangent_spheres_.begin();it!=d3_ids_and_tangent_spheres_.end() && no_conflict_with_other_d3;it++)
							{
								no_conflict_with_other_d3=no_conflict_with_other_d3 && !sphere_intersects_sphere(tangents[i], spheres_->at(it->first));
							}
							if(no_conflict_with_other_d3)
							{
								valid_tangents.push_back(tangents[i]);
							}
						}
					}
					if(!valid_tangents.empty())
					{
						return std::make_pair(true, valid_tangents);
					}
				}
			}
		}
		return std::make_pair(false, std::vector<SimpleSphere>());
	}

	void set_d2_and_unset_d3(const std::size_t d2_id, const SimpleSphere& d2_tangent_sphere)
	{
		d2_id_=d2_id;
		d2_tangent_sphere_=d2_tangent_sphere;
		d3_ids_and_tangent_spheres_.clear();
		die_if_invalid_d2();
	}

	void add_d3(const std::size_t d3_id, const std::vector<SimpleSphere>& d3_tangent_spheres)
	{
		d3_ids_and_tangent_spheres_[d3_id]=d3_tangent_spheres;
		die_if_invalid_d3();
	}

	void unset_d2_and_d3()
	{
		d2_id_=npos;
		d2_tangent_sphere_=SimpleSphere();
		d3_ids_and_tangent_spheres_.clear();
	}

	void unset_d3()
	{
		d3_ids_and_tangent_spheres_.clear();
	}

	std::vector< std::pair<Quadruple, SimpleSphere> > produce_quadruples() const
	{
		std::vector< std::pair<Quadruple, SimpleSphere> > children;
		if(d2_id_!=npos)
		{
			children.push_back(std::make_pair(Quadruple(abc_ids_, d2_id_), d2_tangent_sphere_));
		}
		for(ContainerForD3::const_iterator it=d3_ids_and_tangent_spheres_.begin();it!=d3_ids_and_tangent_spheres_.end();it++)
		{
			const std::size_t d3_id=it->first;
			const std::vector<SimpleSphere>& d3_tangent_spheres=it->second;
			for(std::size_t i=0;i<d3_tangent_spheres.size();i++)
			{
				children.push_back(std::make_pair(Quadruple(abc_ids_, d3_id), d3_tangent_spheres[i]));
			}
		}
		return children;
	}

	std::vector<ApolloniusFace> produce_faces() const
	{
		std::vector<ApolloniusFace> children;
		if(d2_id_!=npos)
		{
			for(int i=0;i<3;i++)
			{
				children.push_back(ApolloniusFace(*spheres_,
						Triple(abc_ids_.exclude(i), d2_id_),
						abc_ids_.get(i),
						d2_tangent_sphere_));
			}
		}
		for(ContainerForD3::const_iterator it=d3_ids_and_tangent_spheres_.begin();it!=d3_ids_and_tangent_spheres_.end();it++)
		{
			const std::size_t d3_id=it->first;
			const std::vector<SimpleSphere>& d3_tangent_spheres=it->second;
			for(int i=0;i<3;i++)
			{
				children.push_back(ApolloniusFace(*spheres_,
						Triple(abc_ids_.exclude(i), d3_id),
						abc_ids_.get(i),
						d3_tangent_spheres.front()));
				if(d3_tangent_spheres.size()>1)
				{
					children.back().set_d2_and_unset_d3(abc_ids_.get(i), d3_tangent_spheres.back());
				}
			}
		}
		return children;
	}

private:
	static std::size_t select_free_tangent_plane_id(
			const Sphere& a,
			const Sphere& b,
			const Sphere& c,
			const std::vector< std::pair<SimplePoint, SimplePoint> >& tangent_planes,
			const Sphere& d1,
			const SimpleSphere& d1_tangent_sphere)
	{
		if(tangent_planes.size()==2)
		{
			const int h0=halfspace_of_sphere(tangent_planes[0].first, tangent_planes[0].second, d1);
			const int h1=halfspace_of_sphere(tangent_planes[1].first, tangent_planes[1].second, d1);
			if(h0>-1 && h1==-1)
			{
				return 1;
			}
			else if(h1>-1 && h0==-1)
			{
				return 0;
			}
			else if(h0==-1 && h1==-1)
			{
				return npos;
			}
			else
			{
				const SimplePoint normal=plane_normal_from_three_points<SimplePoint>(a, b, c);
				const int th=halfspace_of_point(a, normal, d1_tangent_sphere);
				const int th0=halfspace_of_point(a, normal, tangent_planes[0].first+tangent_planes[0].second);
				const int th1=halfspace_of_point(a, normal, tangent_planes[1].first+tangent_planes[1].second);
				if(th0==th && th1!=th)
				{
					return 1;
				}
				else if(th1==th && th0!=th)
				{
					return 0;
				}
				else
				{
					return npos;
				}
			}
		}
		return npos;
	}

	static SimplePoint calculate_centroid(const Sphere& a, const Sphere& b, const Sphere& c)
	{
		return ((custom_point_from_object<SimplePoint>(a)+custom_point_from_object<SimplePoint>(b)+custom_point_from_object<SimplePoint>(c))/3);
	}

	template<typename InputSphereType>
	bool sphere_is_on_free_plane(const InputSphereType& x) const
	{
		return (free_tangent_plane_id_==npos || halfspace_of_sphere(tangent_planes_[free_tangent_plane_id_].first, tangent_planes_[free_tangent_plane_id_].second, x)>-1);
	}

	template<typename InputSphereType>
	bool sphere_may_contain_inner_sphere(const InputSphereType& x) const
	{
		if(tangent_planes_.size()==2)
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
		else if(tangent_stick_.first!=NULL && tangent_stick_.second!=NULL)
		{
			if(!stick_intersects_sphere(*tangent_stick_.first, *tangent_stick_.second, x))
			{
				return false;
			}
			return true;
		}
		else
		{
			throw std::logic_error("Missing face tangency information");
			return false;
		}
	}

	template<typename InputSphereType>
	bool sphere_is_inner(const InputSphereType& x) const
	{
		if(tangent_planes_.size()==2)
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
		else if(tangent_stick_.first!=NULL && tangent_stick_.second!=NULL)
		{
			return stick_contains_sphere(*tangent_stick_.first, *tangent_stick_.second, x);
		}
		else
		{
			throw std::logic_error("Missing face tangency information");
			return false;
		}
	}

	template<typename InputSphereType>
	bool sphere_is_outside_the_bounding_sphere_of_d1_and_d2(const InputSphereType& x) const
	{
		if(d2_id_!=npos)
		{
			const SimpleSphere allowed_volume=custom_sphere_from_point<SimpleSphere>(centroid_,
					std::max(maximal_distance_from_point_to_sphere(centroid_, d1_tangent_sphere_),
							maximal_distance_from_point_to_sphere(centroid_, d2_tangent_sphere_)));
			if(!sphere_intersects_sphere(allowed_volume, x))
			{
				return true;
			}
		}
		return false;
	}

	void die_if_invalid_d2()
	{
		if(d2_id_!=npos)
		{
			if(abc_ids_.contains(d2_id_))
			{
				throw std::logic_error("Invalid d2 ID");
			}

			const Sphere& d2=spheres_->at(d2_id_);

			if(sphere_intersects_sphere(d1_tangent_sphere_, d2))
			{
				throw std::logic_error("The d1 tangent sphere intersects d2");
			}

			if(!check_spheres_tangent(*a_, *b_, *c_, d2, d2_tangent_sphere_))
			{
				throw std::logic_error("Invalid d2 tangent sphere");
			}

			if(sphere_intersects_sphere(d2_tangent_sphere_, *d1_))
			{
				throw std::logic_error("The d2 tangent sphere intersects d1");
			}

			if(d1_id_==d2_id_ && spheres_equal(d1_tangent_sphere_, d2_tangent_sphere_))
			{
				throw std::logic_error("The d1 and d2 are equal and have the same tangent sphere");
			}
		}
	}

	void die_if_invalid_d3() const
	{
		if(!(can_have_d3_ || d3_ids_and_tangent_spheres_.empty()))
		{
			throw std::logic_error("Forbidden d3 are present");
		}

		for(ContainerForD3::const_iterator it=d3_ids_and_tangent_spheres_.begin();it!=d3_ids_and_tangent_spheres_.end();it++)
		{
			const std::size_t d3_id=it->first;
			const std::vector<SimpleSphere>& d3_tangent_spheres=it->second;

			if(d3_id==npos || d3_id==d1_id_ || d3_id==d2_id_ || abc_ids_.contains(d3_id))
			{
				throw std::logic_error("Invalid d3 ID");
			}

			if(d3_tangent_spheres.empty())
			{
				throw std::logic_error("No d3 tangent spheres");
			}

			const Sphere& d3=spheres_->at(d3_id);

			if(sphere_intersects_sphere(d1_tangent_sphere_, d3))
			{
				throw std::logic_error("The d1 tangent sphere intersects d3");
			}

			if(d2_id_!=npos)
			{
				if(sphere_intersects_sphere(d2_tangent_sphere_, d3))
				{
					throw std::logic_error("The d2 tangent sphere intersects d3");
				}
			}

			for(ContainerForD3::const_iterator jt=d3_ids_and_tangent_spheres_.begin();jt!=d3_ids_and_tangent_spheres_.end();jt++)
			{
				if(jt->first!=d3_id)
				{
					for(std::size_t i=0;i<jt->second.size();i++)
					{
						if(sphere_intersects_sphere(jt->second[i], d3))
						{
							throw std::logic_error("One of the other d3 tangent sphere intersects d3");
						}
					}
				}
			}

			for(std::size_t i=0;i<d3_tangent_spheres.size();i++)
			{
				const SimpleSphere& d3_tangent_sphere=d3_tangent_spheres[i];

				if(!check_spheres_tangent(*a_, *b_, *c_, d3, d3_tangent_sphere))
				{
					throw std::logic_error("Invalid d3 tangent sphere");
				}

				if(sphere_intersects_sphere(d3_tangent_sphere, *d1_))
				{
					throw std::logic_error("The d3 tangent sphere intersects d1");
				}

				if(d2_id_!=npos)
				{
					const Sphere& d2=spheres_->at(d2_id_);
					if(sphere_intersects_sphere(d3_tangent_sphere, d2))
					{
						throw std::logic_error("The d3 tangent sphere intersects d2");
					}
				}

				for(ContainerForD3::const_iterator jt=d3_ids_and_tangent_spheres_.begin();jt!=d3_ids_and_tangent_spheres_.end();jt++)
				{
					if(jt->first!=d3_id)
					{
						if(sphere_intersects_sphere(d3_tangent_sphere, spheres_->at(jt->first)))
						{
							throw std::logic_error("The d3 tangent sphere intersects other d3");
						}
					}
				}
			}
		}
	}

	const std::vector<Sphere>* spheres_;
	Triple abc_ids_;
	const Sphere* a_;
	const Sphere* b_;
	const Sphere* c_;
	std::size_t d1_id_;
	const Sphere* d1_;
	SimpleSphere d1_tangent_sphere_;
	std::vector< std::pair<SimplePoint, SimplePoint> > tangent_planes_;
	std::pair<const Sphere*, const Sphere*> tangent_stick_;
	bool has_valid_tangency_information_;
	bool can_have_d2_;
	std::size_t free_tangent_plane_id_;
	SimplePoint centroid_;
	bool can_have_d3_;

	std::size_t d2_id_;
	SimpleSphere d2_tangent_sphere_;
	std::map< std::size_t, std::vector<SimpleSphere> > d3_ids_and_tangent_spheres_;
};

}

#endif /* APOLLONIUS_FACE_H_ */
