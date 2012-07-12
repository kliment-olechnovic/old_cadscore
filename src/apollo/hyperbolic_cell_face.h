#ifndef HYPERBOLIC_CELL_FACE_H_
#define HYPERBOLIC_CELL_FACE_H_

#include <list>
#include <vector>

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

	template<typename SphereType>
	static Contour construct_contour(const SphereType& a, const SphereType& b, std::vector<const SphereType*> cs, const double probe, const double step)
	{
		Contour contour=init_contour(a, b, probe, step);
		for(std::size_t i=0;i<cs.size() && !contour.empty();i++)
		{
			update_contour(a, b, (*(cs[i])), step, contour);
		}
		return contour;
	}

private:
	static SimplePoint any_normal_of_vector(const SimplePoint& a)
	{
		SimplePoint b=a;
		if(!equal(b.x, 0.0)) { b.x=0.0-b.x; } else if(!equal(b.y, 0.0)) { b.y=0.0-b.y; } else { b.z=0.0-b.z; }
		return (a&b).unit();
	}

	template<typename SphereType>
	static Contour init_contour(const SphereType& a, const SphereType& b, const double probe, const double step)
	{
		Contour contour;
		const SimpleSphere as=custom_sphere_from_point<SimpleSphere>(a, a.r+probe);
		const SimpleSphere bs=custom_sphere_from_point<SimpleSphere>(b, b.r+probe);
		if(sphere_intersects_sphere(as, bs))
		{
			const SimpleSphere ic=spheres_intersection_circle<SimpleSphere>(as, bs);
			Rotation rotation(sub_of_points<SimplePoint>(b, a).unit(), 0);
			const SimplePoint first_point=any_normal_of_vector(rotation.axis)*ic.r;
			const double angle_step=std::max(std::min(360*(step/(2*PI*ic.r)), 60.0), 5.0);
			for(;rotation.angle<360;rotation.angle+=angle_step)
			{
				contour.push_back(custom_point_from_object<SimplePoint>(ic)+rotation.rotate<SimplePoint>(first_point));
			}
		}
		return contour;
	}

	template<typename SphereType>
	static void update_contour(const SphereType& a, const SphereType& b, const SphereType& c, const double step, Contour& contour)
	{
		//
	}
};

}

#endif /* HYPERBOLIC_CELL_FACE_H_ */
