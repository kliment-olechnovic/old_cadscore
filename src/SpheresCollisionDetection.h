#ifndef SPHERESCOLLISIONDETECTION_H_
#define SPHERESCOLLISIONDETECTION_H_

class SpheresCollisionDetection
{
public:
	template<typename SpheresMapType>
	static bool check_for_collision(const SpheresMapType& spheres_map, const typename SpheresMapType::Sphere& sphere, bool only_containment)
	{
		typedef typename SpheresMapType::Set Set;
		const Set candidates=spheres_map.find_potential_intersections(sphere);
		for(typename Set::const_iterator it=candidates.begin();it!=candidates.end();it++)
		{
			const typename SpheresMapType::Sphere* s=(*it);
			if(only_containment ? s->contains(sphere) : s->distance_to(sphere)<0)
			{
				return true;
			}
		}
		return false;
	}

	template<typename SpheresMapType, typename SpheresContainerType>
	static SpheresContainerType filter_colliders(const SpheresMapType& spheres_map, const SpheresContainerType& candidates, bool only_containment)
	{
		SpheresContainerType result;
		for(typename SpheresContainerType::const_iterator it=candidates.begin();it!=candidates.end();it++)
		{
			const typename SpheresMapType::Sphere* s=(*it);
			if(check_for_any_collision(spheres_map, (*s), only_containment))
			{
				result.insert(s);
			}
		}
		return result;
	}
};

#endif /* SPHERESCOLLISIONDETECTION_H_ */
