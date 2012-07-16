#ifndef HYPERBOLIC_CELL_FACE_H_
#define HYPERBOLIC_CELL_FACE_H_

#include <list>
#include <vector>
#include <deque>

#include "points_basic_operations.h"
#include "spheres_basic_operations.h"
#include "hyperboloids_basic_operations.h"
#include "rotation.h"

namespace apollo
{

class HyperbolicCellFace
{
public:
	typedef std::list<SimplePoint> Contour;

	HyperbolicCellFace()
	{
	}

	template<typename SphereType>
	bool construct_contour(
			const SphereType& a,
			const SphereType& b,
			std::vector<const SphereType*> list_of_c,
			const double probe,
			const double step)
	{
		intersection_circle_=SimpleSphere();
		contour_.clear();

		const SimpleSphere a_expanded=custom_sphere_from_point<SimpleSphere>(a, a.r+probe);
		const SimpleSphere b_expanded=custom_sphere_from_point<SimpleSphere>(b, b.r+probe);
		if(sphere_intersects_sphere(a_expanded, b_expanded))
		{
			intersection_circle_=spheres_intersection_circle<SimpleSphere>(a_expanded, b_expanded);

			{
				Rotation rotation(sub_of_points<SimplePoint>(b, a).unit(), 0);
				const SimplePoint first_point=any_normal_of_vector(rotation.axis)*intersection_circle_.r;
				const double angle_step=std::max(std::min(360*(step/(2*PI*intersection_circle_.r)), 60.0), 5.0);
				for(;rotation.angle<360;rotation.angle+=angle_step)
				{
					contour_.push_back(custom_point_from_object<SimplePoint>(intersection_circle_)+rotation.rotate<SimplePoint>(first_point));
				}
			}

			for(std::size_t i=0;i<list_of_c.size() && !contour_.empty();i++)
			{
				update_contour(a, b, (*(list_of_c[i])), step, contour_);
			}
		}
		return (!contour_.empty());
	}

	Contour contour() const
	{
		return contour_;
	}

private:
	static SimplePoint any_normal_of_vector(const SimplePoint& a)
	{
		SimplePoint b=a;
		if(!equal(b.x, 0.0)) { b.x=0.0-b.x; } else if(!equal(b.y, 0.0)) { b.y=0.0-b.y; } else { b.z=0.0-b.z; }
		return (a&b).unit();
	}

	template<typename SphereType>
	static void update_contour(const SphereType& a,
			const SphereType& b,
			const SphereType& c,
			const double step,
			Contour& contour)
	{
		std::deque< std::pair<Contour::iterator, bool> > intersection_iterators;

		{
			Contour::iterator left_it=contour.begin();
			while(left_it!=contour.end())
			{
				Contour::iterator right_it=left_it;
				++right_it;
				if(right_it==contour.end())
				{
					right_it=contour.begin();
				}
				const SimplePoint& left_point=*left_it;
				const SimplePoint& right_point=*right_it;
				const bool left_halfspace=less(minimal_distance_from_point_to_sphere(left_point, c), minimal_distance_from_point_to_sphere(left_point, a));
				const bool right_halfspace=less(minimal_distance_from_point_to_sphere(right_point, c), minimal_distance_from_point_to_sphere(right_point, a));
				if(left_halfspace!=right_halfspace)
				{
					const double pos=intersect_vector_with_hyperboloid(left_point, right_point, a, c);
					if(pos>0.0)
					{
						const SimplePoint intersection_point=left_point+((right_point-left_point).unit()*pos);
						Contour::iterator intersection_iterator=contour.insert(right_it, intersection_point);
						intersection_iterators.push_back(std::make_pair(intersection_iterator, right_halfspace));
					}
				}
				if(right_it==contour.begin())
				{
					left_it=contour.end();
				}
				else
				{
					left_it=right_it;
				}
			}
		}

		if(intersection_iterators.size()>0 && intersection_iterators.size()%2==0)
		{
			if(!intersection_iterators.front().second)
			{
				intersection_iterators.push_front(intersection_iterators.back());
				intersection_iterators.pop_back();
			}

			bool valid_sequence=true;
			for(std::size_t k=0;k<intersection_iterators.size() && valid_sequence;k+=2)
			{
				valid_sequence=valid_sequence && (intersection_iterators[k].second && !intersection_iterators[k+1].second);
			}

			if(valid_sequence)
			{
				for(std::size_t j=0;j<intersection_iterators.size();j++)
				{
					SimplePoint& point=*(intersection_iterators[j].first);
					for(int i=0;i<10;i++)
					{
						point=project_point_on_hyperboloid(point, b, c);
						point=project_point_on_hyperboloid(point, a, c);
						point=project_point_on_hyperboloid(point, a, b);
					}
				}

				for(std::size_t k=0;k<intersection_iterators.size();k+=2)
				{
					Contour::iterator first_intersection=intersection_iterators[k].first;
					Contour::iterator second_intersection=intersection_iterators[k+1].first;

					{
						Contour::iterator it=first_intersection;
						++it;
						while(it!=second_intersection && it!=contour.end())
						{
							it=contour.erase(it);
						}
						if(it!=second_intersection)
						{
							it=contour.begin();
							while(it!=second_intersection && it!=contour.end())
							{
								it=contour.erase(it);
							}
						}
					}

					{
						const SimplePoint& left_point=*(first_intersection);
						const SimplePoint& right_point=*(second_intersection);
						const double distance=distance_from_point_to_point(left_point, right_point);
						const SimplePoint direction=(right_point-left_point).unit();
						for(double pos=step;pos<distance;pos+=step)
						{
							SimplePoint p=left_point+(direction*pos);
							for(int i=0;i<10;i++)
							{
								p=project_point_on_hyperboloid(p, a, c);
								p=project_point_on_hyperboloid(p, a, b);
								p=project_point_on_hyperboloid(p, b, c);
							}
							contour.insert(second_intersection, p);
						}
					}
				}
			}
		}
	}

	SimpleSphere intersection_circle_;
	Contour contour_;
};

}

#endif /* HYPERBOLIC_CELL_FACE_H_ */
