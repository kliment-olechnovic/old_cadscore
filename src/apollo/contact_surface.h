#ifndef CONTACT_SURFACE_H_
#define CONTACT_SURFACE_H_

#include <vector>

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
	};

	typedef std::vector< std::pair<std::size_t, Triangle> > Surface;

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
			surfaces.push_back(construct_surface(sih, spheres, collect_influences(sih, spheres, neighbours)));
		}
		return surfaces;
	}

private:
	ContactSurface()
	{
	}

	template<typename SphereType>
	static std::vector< std::vector<std::size_t> > collect_influences(
			const SubdividedIcosahedron& sih,
			const std::vector<SphereType>& spheres,
			const std::vector<size_t>& neighbours)
	{
		std::vector< std::vector<std::size_t> > influences(sih.vertices().size());
		const double treshold=sih.edge_length_estimate();
		for(std::size_t i=0;i<influences.size();i++)
		{
			double min_distance=minimal_distance_from_point_to_sphere(sih.vertices().at(i), spheres.at(neighbours.at(0)));
			influences[i].push_back(neighbours.at(0));
			for(std::size_t j=1;j<neighbours.size();j++)
			{
				double distance=minimal_distance_from_point_to_sphere(sih.vertices().at(i), spheres.at(neighbours[j]));
				if(less(distance-treshold, min_distance))
				{
					if(less(distance, min_distance))
					{
						if(!less(min_distance-treshold, distance))
						{
							influences[i].clear();
						}
						min_distance=distance;
					}
					influences[i].push_back(neighbours[j]);
				}
			}
		}
		return influences;
	}

	template<typename SphereType>
	static Surface construct_surface(
			const SubdividedIcosahedron& sih,
			const std::vector<SphereType>& spheres,
			const std::vector< std::vector<std::size_t> >& influences)
	{
		Surface colored_triangles;
		for(std::size_t e=0;e<sih.triples().size();e++)
		{
			const Triple& triple=sih.triples()[e];
			const std::size_t a=triple.get(0);
			const std::size_t b=triple.get(1);
			const std::size_t c=triple.get(2);
			const Triangle big_triangle(sih.vertices()[a], sih.vertices()[b], sih.vertices()[c]);
			if(influences[a].size()==1 && influences[b].size()==1 && influences[c].size()==1
					&& influences[a][0]==influences[b][0] && influences[a][0]==influences[c][0])
			{
				colored_triangles.push_back(std::make_pair(influences[a][0], big_triangle));
			}
			else
			{
				std::set<std::size_t> ids;
				for(int i=0;i<3;i++)
				{
					const std::size_t n=triple.get(i);
					for(std::size_t j=0;j<influences[n].size();j++)
					{
						ids.insert(influences[n][j]);
					}
				}

				for(std::set<std::size_t>::const_iterator it=ids.begin();it!=ids.end();++it)
				{
					std::vector<Triangle> triangles;
					triangles.push_back(big_triangle);
					for(std::set<std::size_t>::const_iterator jt=ids.begin();jt!=ids.end() && !triangles.empty();++jt)
					{
						if((*it)!=(*jt))
						{
							std::vector<Triangle> new_triangles;
							for(std::size_t i=0;i<triangles.size();i++)
							{
								const std::vector<SimplePoint> control_points=intersect_triangle_with_hyperboloid(triangles[i].a, triangles[i].b, triangles[i].c, spheres[(*it)], spheres[(*jt)]);
								if(control_points.size()>=3)
								{
									new_triangles.push_back(Triangle(control_points[0], control_points[1], control_points[2]));
								}
								if(control_points.size()>=4)
								{
									new_triangles.push_back(Triangle(control_points[0], control_points[2], control_points[3]));
								}
							}
							triangles=new_triangles;
						}
					}
					for(std::size_t i=0;i<triangles.size();i++)
					{
						colored_triangles.push_back(std::make_pair((*it), triangles[i]));
					}
				}
			}
		}
		return colored_triangles;
	}
};

}

#endif /* CONTACT_SURFACE_H_ */
