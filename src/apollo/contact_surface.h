#ifndef CONTACT_SURFACE_H_
#define CONTACT_SURFACE_H_

#include <vector>
#include <set>
#include <map>

#include "points_basic_operations.h"
#include "spheres_basic_operations.h"
#include "subdivided_icosahedron.h"
#include "hyperboloids_basic_operations.h"
#include "tuples.h"

namespace apollo
{

class ContactSurface
{
public:
	struct Triangle
	{
		SimplePoint a;
		SimplePoint b;
		SimplePoint c;

		Triangle(const SimplePoint& a, const SimplePoint& b, const SimplePoint& c) : a(a), b(b), c(c)
		{
		}

		double area() const
		{
			return triangle_area(a, b, c);
		}
	};

	typedef std::vector< std::pair<std::size_t, Triangle> > Surface;

	typedef std::map<std::size_t, double> SurfaceArea;

	template<typename SphereType>
	static std::vector<Surface> construct_surfaces(
			const std::vector<SphereType>& spheres,
			const std::vector< std::vector<std::size_t> >& graph,
			const std::size_t subdivision_depth,
			const double probe_radius)
	{
		std::vector<Surface> surfaces;
		SubdividedIcosahedron sih(subdivision_depth);
		surfaces.reserve(graph.size());
		for(std::size_t i=0;i<graph.size();i++)
		{
			std::vector<std::size_t> neighbours=graph[i];
			neighbours.push_back(i);
			sih.fit_into_sphere(spheres[i], spheres[i].r+probe_radius);
			surfaces.push_back(construct_surface(sih, spheres, collect_influences(sih, spheres, i, neighbours)));
		}
		return surfaces;
	}

private:
	ContactSurface()
	{
	}

	template<typename SphereType>
	static std::vector<std::size_t> collect_influences(
			const SubdividedIcosahedron& sih,
			const std::vector<SphereType>& spheres,
			const std::size_t self_id,
			const std::vector<size_t>& neighbours)
	{
		std::vector<std::size_t> influences(sih.vertices().size());
		for(std::size_t i=0;i<influences.size();i++)
		{
			double min_distance=minimal_distance_from_point_to_sphere(sih.vertices().at(i), spheres.at(self_id));
			influences[i]=self_id;
			for(std::size_t j=0;j<neighbours.size();j++)
			{
				double distance=minimal_distance_from_point_to_sphere(sih.vertices().at(i), spheres.at(neighbours[j]));
				if(distance<min_distance)
				{
					min_distance=distance;
					influences[i]=neighbours[j];
				}
			}
		}
		return influences;
	}

	template<typename SphereType>
	static Surface construct_surface(
			const SubdividedIcosahedron& sih,
			const std::vector<SphereType>& spheres,
			const std::vector<std::size_t>& influences)
	{
		Surface colored_triangles;
		for(std::size_t e=0;e<sih.triples().size();e++)
		{
			const Triple& triple=sih.triples()[e];
			const std::size_t a=triple.get(0);
			const std::size_t b=triple.get(1);
			const std::size_t c=triple.get(2);
			if(influences[a]==influences[b] && influences[a]==influences[c])
			{
				colored_triangles.push_back(std::make_pair(influences[a], Triangle(sih.vertices()[a], sih.vertices()[b], sih.vertices()[c])));
			}
			else if(influences[a]!=influences[b] && influences[a]!=influences[c] && influences[b]!=influences[c])
			{
				const SimplePoint& pa=sih.vertices()[a];
				const SimplePoint& pb=sih.vertices()[b];
				const SimplePoint& pc=sih.vertices()[c];

				const SimplePoint a_b_border=pa+((pb-pa).unit()*intersect_vector_with_hyperboloid(pa, pb, spheres[influences[a]], spheres[influences[b]]));
				const SimplePoint a_c_border=pa+((pc-pa).unit()*intersect_vector_with_hyperboloid(pa, pc, spheres[influences[a]], spheres[influences[c]]));
				const SimplePoint b_c_border=pb+((pc-pb).unit()*intersect_vector_with_hyperboloid(pb, pc, spheres[influences[b]], spheres[influences[c]]));

				const SimplePoint middle=(a_b_border+a_c_border+b_c_border)/3;

				colored_triangles.push_back(std::make_pair(influences[a], Triangle(pa, a_b_border, a_c_border)));
				colored_triangles.push_back(std::make_pair(influences[a], Triangle(middle, a_b_border, a_c_border)));

				colored_triangles.push_back(std::make_pair(influences[b], Triangle(pb, a_b_border, b_c_border)));
				colored_triangles.push_back(std::make_pair(influences[b], Triangle(middle, a_b_border, b_c_border)));

				colored_triangles.push_back(std::make_pair(influences[c], Triangle(pc, a_c_border, b_c_border)));
				colored_triangles.push_back(std::make_pair(influences[c], Triangle(middle, a_c_border, b_c_border)));
			}
			else
			{
				std::size_t s=a;
				std::size_t d1=b;
				std::size_t d2=c;
				if(influences[b]!=influences[a] && influences[b]!=influences[c])
				{
					s=b;
					d1=a;
					d2=c;
				}
				else if(influences[c]!=influences[a] && influences[c]!=influences[b])
				{
					s=c;
					d1=a;
					d2=b;
				}

				const SimplePoint& ps=sih.vertices()[s];
				const SimplePoint& pd1=sih.vertices()[d1];
				const SimplePoint& pd2=sih.vertices()[d2];

				const SimplePoint s_d1_border=ps+((pd1-ps).unit()*intersect_vector_with_hyperboloid(ps, pd1, spheres[influences[s]], spheres[influences[d1]]));
				const SimplePoint s_d2_border=ps+((pd2-ps).unit()*intersect_vector_with_hyperboloid(ps, pd2, spheres[influences[s]], spheres[influences[d2]]));

				colored_triangles.push_back(std::make_pair(influences[s], Triangle(ps, s_d1_border, s_d2_border)));
				colored_triangles.push_back(std::make_pair(influences[d1], Triangle(pd1, s_d1_border, s_d2_border)));
				colored_triangles.push_back(std::make_pair(influences[d2], Triangle(pd2, pd1, s_d2_border)));
			}
		}
		return colored_triangles;
	}
};

}

#endif /* CONTACT_SURFACE_H_ */
