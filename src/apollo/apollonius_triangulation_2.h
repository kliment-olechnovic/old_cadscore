#ifndef APOLLO_APOLLONIUS_TRIANGULATION_2_H_
#define APOLLO_APOLLONIUS_TRIANGULATION_2_H_

#include <iostream>
#include <vector>
#include <deque>
#include <tr1/unordered_map>
#include <tr1/functional>

#include "apollonius_face_2.h"

namespace apollo
{

template<typename SpheresHierarchyType>
class ApolloniusTriangulation2
{
public:
	typedef SpheresHierarchyType Hierarchy;
	typedef typename Hierarchy::Sphere Sphere;
	typedef std::tr1::unordered_map<Quadruple, std::vector<SimpleSphere>, Quadruple::HashFunctor> QuadruplesMap;

	static QuadruplesMap find_quadruples(const Hierarchy& hierarchy, bool enable_searching_for_e)
	{
		QuadruplesMap quadruples_map;
		std::deque<Face> stack=find_first_faces(hierarchy);
		std::tr1::unordered_map<Triple, std::size_t, Triple::HashFunctor> stack_map;
		for(std::size_t i=0;i<stack.size();i++)
		{
			stack_map[stack[i].abc_ids()]=i;
		}
		while(!stack.empty())
		{
			Face face=stack.back();
			stack.pop_back();
			stack_map.erase(face.abc_ids());
		}
		return quadruples_map;
	}

	static int& monitoring_level_reference()
	{
		static int monitoring_level=0;
		return monitoring_level;
	}

	static int monitoring_level()
	{
		return monitoring_level_reference();
	}

private:
	typedef ApolloniusFace2<Sphere> Face;

	struct checkers_for_any_d
	{
		struct NodeChecker
		{
			const Face& face;
			const std::size_t d_number;

			NodeChecker(const Face& target, const std::size_t d_number) : face(target), d_number(d_number)
			{
			}

			bool operator()(const SimpleSphere& sphere) const
			{
				return face.sphere_may_contain_candidate_for_d(sphere, d_number);
			}
		};

		struct LeafChecker
		{
			Face& face;
			const std::size_t d_number;

			LeafChecker(Face& target, const std::size_t d_number) : face(target), d_number(d_number)
			{
			}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& /*sphere*/)
			{
				const std::vector<SimpleSphere> check_result=face.check_candidate_for_d(id, d_number);
				if(check_result.size()==1)
				{
					face.set_d(id, d_number, check_result.front());
					return std::make_pair(true, true);
				}
				return std::make_pair(false, false);
			}
		};
	};

	struct checkers_for_valid_d
	{
		struct NodeChecker
		{
			const Face& face;
			const std::size_t d_number;

			NodeChecker(const Face& target, const std::size_t d_number) : face(target), d_number(d_number)
			{
			}

			bool operator()(const SimpleSphere& sphere) const
			{
				return (face.has_d(d_number) && sphere_intersects_sphere(sphere, face.get_d_tangent_sphere(d_number)));
			}
		};

		struct LeafChecker
		{
			Face& face;
			const std::size_t d_number;

			std::tr1::unordered_set<std::size_t> visited;

			LeafChecker(const Face& target, const std::size_t d_number) : face(target), d_number(d_number)
			{
			}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& sphere)
			{
				if(face.has_d(d_number) && sphere_intersects_sphere(sphere, face.get_d_tangent_sphere(d_number)))
				{
					const std::vector<SimpleSphere> check_result=face.check_candidate_for_d(id, d_number);
					if(check_result.size()==1)
					{
						face.set_d(id, d_number, check_result.front());
						return std::make_pair(true, true);
					}
					else
					{
						return std::make_pair(true, false);
					}
				}
				return std::make_pair(false, false);
			}
		};
	};

	struct checkers_for_valid_e
	{
		struct NodeChecker
		{
			const Face& face;

			NodeChecker(const Face& target) : face(target)
			{
			}

			bool operator()(const SimpleSphere& sphere) const
			{
				return face.sphere_may_contain_candidate_for_e(sphere);
			}
		};

		struct LeafChecker
		{
			Face& face;
			const Hierarchy& hierarchy;

			LeafChecker(Face& target, const Hierarchy& hierarchy) : face(target), hierarchy(hierarchy)
			{
			}

			std::pair<bool, bool> operator()(const std::size_t id, const Sphere& /*sphere*/)
			{
				const std::vector<SimpleSphere> check_result=face.check_candidate_for_e(id);
				bool e_added=false;
				for(std::size_t i=0;i<check_result.second.size();i++)
				{
					if(hierarchy.find_any_collision(check_result[i]).empty())
					{
						face.add_e(id, check_result[i]);
						e_added=true;
					}
				}
				return std::make_pair(e_added, false);
			}
		};
	};

	static std::deque<Face> find_first_faces(const Hierarchy& hierarchy)
	{
		const std::vector<Sphere>& spheres=hierarchy.spheres();
		std::deque<Face> result;
		int tries_count=0;
		if(!spheres.empty())
		{
			const std::vector<std::size_t> traversal=sort_objects_by_functor_result(spheres, std::tr1::bind(minimal_distance_from_sphere_to_sphere<Sphere, Sphere>, spheres.front(), std::tr1::placeholders::_1));
			for(std::size_t u=4;u<traversal.size();u++)
			{
				for(std::size_t a=0;a<u;a++)
				{
					for(std::size_t b=a+1;b<u;b++)
					{
						for(std::size_t c=b+1;c<u;c++)
						{
							for(std::size_t d=((a+1<u && b+1<u && c+1<u) ? (u-1) : (c+1));d<u;d++)
							{
								tries_count++;
								Quadruple quadruple=make_quadruple(traversal[a], traversal[b], traversal[c], traversal[d]);
								std::vector<SimpleSphere> tangents=construct_spheres_tangent_sphere<SimpleSphere>(spheres[quadruple.get(0)], spheres[quadruple.get(1)], spheres[quadruple.get(2)], spheres[quadruple.get(3)]);
								if(tangents.size()==1 && hierarchy.find_any_collision(tangents.front()).empty())
								{
									for(int i=0;i<4;i++)
									{
										result.push_back(Face(spheres, quadruple.exclude(i)));
									}
									if(monitoring_level()>0)
									{
										std::clog << "brute " << tries_count << "\n";
									}
									return result;
								}
							}
						}
					}
				}
			}
		}
		if(monitoring_level()>0)
		{
			std::clog << "brute " << tries_count << "\n";
		}
		return result;
	}

	static bool find_valid_d(const Hierarchy& hierarchy, const Face& face, const std::size_t d_number)
	{
		if(!face.has_d(d_number))
		{
			typename checkers_for_any_d::LeafChecker leaf_checker(face, d_number);
			typename checkers_for_any_d::NodeChecker node_checker(face, d_number);
			hierarchy.search(node_checker, leaf_checker);
		}
		if(face.has_d(d_number))
		{
			typename checkers_for_valid_d::NodeChecker node_checker(face, d_number);
			typename checkers_for_valid_d::LeafChecker leaf_checker(face, d_number);
			while(face.has_d(d_number))
			{
				const std::vector<std::size_t> results=hierarchy.search(node_checker, leaf_checker);
				if(results.empty())
				{
					return true;
				}
				else if(face.get_d_id(d_number)!=results.back())
				{
					face.unset_d(d_number);
				}
			}
		}
		return false;
	}

	static bool find_valid_e(const Hierarchy& hierarchy, Face& face)
	{
		typename checkers_for_valid_e::NodeChecker node_checker(face);
		typename checkers_for_valid_e::LeafChecker leaf_checker(face, hierarchy);
		return !hierarchy.search(node_checker, leaf_checker).empty();
	}
};

}

#endif /* APOLLO_APOLLONIUS_TRIANGULATION_2_H_ */
