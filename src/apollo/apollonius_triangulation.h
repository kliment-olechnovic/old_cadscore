#ifndef APOLLONIUS_TRIANGULATION_H_
#define APOLLONIUS_TRIANGULATION_H_

#include <tr1/unordered_set>

#include "spheres_hierarchy.h"
#include "apollonius_face.h"

namespace apollo
{

template<typename SphereType>
class ApolloniusTriangulation
{
public:
	typedef SphereType Sphere;

	ApolloniusTriangulation(const SpheresHierarchy<Sphere>& spheres_hierarchy) : spheres_hierarchy_(spheres_hierarchy)
	{
	}

private:
	const SpheresHierarchy<Sphere>& spheres_hierarchy_;
};

}

#endif /* APOLLONIUS_TRIANGULATION_H_ */
