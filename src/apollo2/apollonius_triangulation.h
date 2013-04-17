#ifndef APOLLO2_APOLLONIUS_TRIANGULATION_2_H_
#define APOLLO2_APOLLONIUS_TRIANGULATION_2_H_

#include "apollonius_triangulation/search_for_valid_quadruples.h"

namespace apollo2
{

class ApolloniusTriangulation
{
public:
	typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;
	typedef apollonius_triangulation::Log Log;

	template<typename SphereType>
	static QuadruplesMap construct(const BoundingSpheresHierarchy<SphereType>& bsh, const bool enable_searching_for_e)
	{
		return apollonius_triangulation::find_valid_quadruples(bsh, enable_searching_for_e);
	}

	static const Log& log()
	{
		return apollonius_triangulation::log_ref();
	}
};

}

#endif /* APOLLO2_APOLLONIUS_TRIANGULATION_2_H_ */
