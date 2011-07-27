#ifndef SPHERESCOLLISIONDETECTION_H_
#define SPHERESCOLLISIONDETECTION_H_

template<typename SpheresMapType>
class SpheresCollisionDetection
{
public:
	typedef SpheresMapType SpheresMap;
	typedef typename SpheresMap::Sphere Sphere;
	typedef typename SpheresMap::Set Set;

	static bool check_for_collision(const SpheresMap& spheres_map, const Sphere& sphere, bool only_containment)
	{
		const Set candidates=spheres_map.find_potential_intersections(sphere);
		for(typename Set::const_iterator it=candidates.begin();it!=candidates.end();it++)
		{
			const Sphere* s=(*it);
			if(only_containment ? s->contains(sphere) : s->intersects_with(sphere)<0)
			{
				return true;
			}
		}
		return false;
	}

	template<typename InputContainerType, typename OutputContainerType>
	static OutputContainerType select_colliders(const SpheresMap& spheres_map, const InputContainerType& candidates, bool only_containment)
	{
		OutputContainerType result;
		for(typename InputContainerType::const_iterator it=candidates.begin();it!=candidates.end();it++)
		{
			const Sphere* s=(*it);
			if(check_for_any_collision(spheres_map, (*s), only_containment))
			{
				result.insert(s);
			}
		}
		return result;
	}
};

#endif /* SPHERESCOLLISIONDETECTION_H_ */
