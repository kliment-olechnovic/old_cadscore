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
	HyperbolicCellFace()
	{
	}

	template<typename SphereType>
	static HyperbolicCellFace construct(
			const SphereType& a,
			const SphereType& b,
			std::vector<const SphereType*> list_of_c,
			const double probe,
			const double step,
			const int projections)
	{
		HyperbolicCellFace hcf;
		hcf.s1_=custom_sphere_from_object<SimpleSphere>(a);
		hcf.s2_=custom_sphere_from_object<SimpleSphere>(b);
		const SimpleSphere a_expanded=custom_sphere_from_point<SimpleSphere>(a, a.r+probe);
		const SimpleSphere b_expanded=custom_sphere_from_point<SimpleSphere>(b, b.r+probe);
		if(sphere_intersects_sphere(a_expanded, b_expanded))
		{
			hcf.intersection_circle_=spheres_intersection_circle<SimpleSphere>(a_expanded, b_expanded);
			std::list<SimplePoint> contour;
			bool rerun=false;
			double using_step=step;
			for(int j=0;j<9 && (j==0 || rerun);j++)
			{
				if(j>0)
				{
					using_step*=1.1;
				}
				contour.clear();
				{
					Rotation rotation(sub_of_points<SimplePoint>(b, a).unit(), 0);
					const SimplePoint first_point=any_normal_of_vector<SimplePoint>(rotation.axis)*hcf.intersection_circle_.r;
					const double angle_step=std::max(std::min(360*(using_step/(2*PI*hcf.intersection_circle_.r)), 60.0), 5.0);
					for(rotation.angle=1.0*j;rotation.angle<360;rotation.angle+=angle_step)
					{
						contour.push_back(custom_point_from_object<SimplePoint>(hcf.intersection_circle_)+rotation.rotate<SimplePoint>(first_point));
					}
				}
				rerun=false;
				for(std::size_t i=0;i<list_of_c.size() && !contour.empty();i++)
				{
					std::size_t intersections_count=update_contour(a, b, (*(list_of_c[i])), using_step, projections, contour);
					if(j>=3 && intersections_count>0 && (intersections_count%2)!=0)
					{
						for(std::list<SimplePoint>::iterator it=contour.begin();it!=contour.end();++it)
						{
							const double delta=epsilon()*j;
							double* coord_ptr=(j%3==0 ? &(it->x) : (j%3==1 ? &(it->y) : &(it->z)));
							(*coord_ptr)+=delta;
						}
						intersections_count=update_contour(a, b, (*(list_of_c[i])), using_step, projections, contour);
					}
					if(intersections_count>0 && (intersections_count%2)!=0)
					{
						rerun=true;
					}
				}
			}
			if(rerun)
			{
				std::cerr << "Incomplete face between two spheres:\n";
				std::cerr << "   " << a.x << " " << a.y << " " << a.z << "\n";
				std::cerr << "   " << b.x << " " << b.y << " " << b.z << "\n";
			}
			if(!contour.empty())
			{
				hcf.contour_points_.reserve(contour.size());
				for(std::list<SimplePoint>::const_iterator it=contour.begin();it!=contour.end();++it)
				{
					hcf.contour_points_.push_back(*it);
				}
				init_mesh(a, b, hcf.contour_points_, hcf.mesh_vertices_, hcf.mesh_triples_);
			}
		}
		return hcf;
	}

	const SimpleSphere& s1() const
	{
		return s1_;
	}

	const SimpleSphere& s2() const
	{
		return s2_;
	}

	const SimpleSphere& intersection_circle() const
	{
		return intersection_circle_;
	}

	const std::vector<SimplePoint>& contour_points() const
	{
		return contour_points_;
	}

	const std::vector<SimplePoint>& mesh_vertices() const
	{
		return mesh_vertices_;
	}

	const std::vector<Triple>& mesh_triples() const
	{
		return mesh_triples_;
	}

	double area() const
	{
		double val=0.0;
		for(std::size_t i=0;i<mesh_triples_.size();i++)
		{
			const Triple& t=mesh_triples_[i];
			val+=triangle_area(mesh_vertices_[t.get(0)], mesh_vertices_[t.get(1)], mesh_vertices_[t.get(2)]);
		}
		return val;
	}

private:
	template<typename SphereType>
	static std::size_t update_contour(
			const SphereType& a,
			const SphereType& b,
			const SphereType& c,
			const double step,
			const int projections,
			std::list<SimplePoint>& contour)
	{
		if(contour.empty())
		{
			return 0;
		}

		std::size_t out_count=0;
		{
			for(std::list<SimplePoint>::const_iterator it=contour.begin();it!=contour.end();++it)
			{
				if(less(minimal_distance_from_point_to_sphere(*it, c), minimal_distance_from_point_to_sphere(*it, a)))
				{
					out_count++;
				}
			}
		}

		if(out_count==contour.size())
		{
			contour.clear();
			return 0;
		}
		else if(out_count==0)
		{
			return 0;
		}

		std::deque< std::pair<std::list<SimplePoint>::iterator, bool> > intersection_iterators;
		{
			std::list<SimplePoint>::iterator left_it=contour.begin();
			while(left_it!=contour.end())
			{
				std::list<SimplePoint>::iterator right_it=left_it;
				++right_it;
				if(right_it==contour.end())
				{
					right_it=contour.begin();
				}
				const SimplePoint& left_point=*left_it;
				const SimplePoint& right_point=*right_it;
				const bool left_halfspace=minimal_distance_from_point_to_sphere(left_point, c)<minimal_distance_from_point_to_sphere(left_point, a);
				const bool right_halfspace=minimal_distance_from_point_to_sphere(right_point, c)<minimal_distance_from_point_to_sphere(right_point, a);
				if(left_halfspace!=right_halfspace)
				{
					const double pos=intersect_vector_with_hyperboloid(left_point, right_point, a, c);
					if(pos>0.0)
					{
						const SimplePoint intersection_point=left_point+((right_point-left_point).unit()*pos);
						std::list<SimplePoint>::iterator intersection_iterator=contour.insert(right_it, intersection_point);
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
					for(int i=0;i<projections;i++)
					{
						point=project_point_on_hyperboloid(point, b, c);
						point=project_point_on_hyperboloid(point, a, c);
						point=project_point_on_hyperboloid(point, a, b);
					}
				}

				for(std::size_t k=0;k<intersection_iterators.size();k+=2)
				{
					std::list<SimplePoint>::iterator first_intersection=intersection_iterators[k].first;
					std::list<SimplePoint>::iterator second_intersection=intersection_iterators[k+1].first;

					{
						std::list<SimplePoint>::iterator it=first_intersection;
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
							for(int i=0;i<projections;i++)
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
		return intersection_iterators.size();
	}

	template<typename SphereType>
	static void init_mesh(
			const SphereType& a,
			const SphereType& b,
			const std::vector<SimplePoint>& contour_points,
			std::vector<SimplePoint>& mesh_vertices,
			std::vector<Triple>& mesh_triples)
	{
		if(contour_points.empty())
		{
			return;
		}

		mesh_vertices=contour_points;
		SimplePoint mass_center=mesh_vertices.front();
		for(std::size_t i=1;i<mesh_vertices.size();i++)
		{
			mass_center=mass_center+mesh_vertices[i];
		}
		mass_center=mass_center/mesh_vertices.size();
		mesh_vertices.push_back(mass_center);

		mesh_triples.reserve(contour_points.size());
		for(std::size_t i=0;i+1<contour_points.size();i++)
		{
			mesh_triples.push_back(make_triple(i, i+1, contour_points.size()));
		}
		mesh_triples.push_back(make_triple(0, contour_points.size()-1, contour_points.size()));
	}

	SimpleSphere s1_;
	SimpleSphere s2_;
	SimpleSphere intersection_circle_;
	std::vector<SimplePoint> contour_points_;
	std::vector<SimplePoint> mesh_vertices_;
	std::vector<Triple> mesh_triples_;
};

}

#endif /* HYPERBOLIC_CELL_FACE_H_ */
