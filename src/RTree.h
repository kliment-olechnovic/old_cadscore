#ifndef RTREE_H_
#define RTREE_H_

#include <limits>

template<typename ObjectType, typename FloatingPointType, template<typename> class ListTypeProxyType>
class RTree
{
public:
	typedef ObjectType Object;
	typedef FloatingPointType FloatingPoint;

	RTree(const Object& initial_object) : root_(NULL)
	{
	}

	~RTree()
	{
		delete root_;
	}

private:
	struct Box
	{
		FloatingPoint x0;
		FloatingPoint x1;
		FloatingPoint y0;
		FloatingPoint y1;
		FloatingPoint z0;
		FloatingPoint z1;

		Box() :
			x0(std::numeric_limits<FloatingPoint>::max()),
			x1(std::numeric_limits<FloatingPoint>::min()),
			y0(std::numeric_limits<FloatingPoint>::max()),
			y1(std::numeric_limits<FloatingPoint>::min()),
			z0(std::numeric_limits<FloatingPoint>::max()),
			z1(std::numeric_limits<FloatingPoint>::min())
		{
		}
	};

	static Box construct_bounding_box(const Object& object)
	{
		const FloatingPoint x=object.x;
		const FloatingPoint y=object.y;
		const FloatingPoint z=object.z;
		const FloatingPoint r=object.r;
		Box box;
		box.x0=(x-r);
		box.x1=(x+r);
		box.y0=(y-r);
		box.y1=(y+r);
		box.z0=(z-r);
		box.z1=(z+r);
		return box;
	}

	static Box merge_boxes(const Box& a, const Box& b)
	{
		Box box;
		box.x0=std::min(a.x0, b.x0);
		box.x1=std::max(a.x1, b.x1);
		box.y0=std::min(a.y0, b.y0);
		box.y1=std::max(a.y1, b.y1);
		box.z0=std::min(a.z0, b.z0);
		box.z1=std::max(a.z1, b.z1);
		return box;
	}

	static Box expand_bounding_box(const Box& box, const Object& object)
	{
		return merge_boxes(box, construct_bounding_box(object));
	}

	struct Node
	{
		typedef typename ListTypeProxyType<Node*>::Type Children;
		typedef typename ListTypeProxyType<const Object*>::Type Objects;
		Box bounds;
		Node* parent;
		Children children;
		Objects objects;

		Node(Node* parent) : parent(parent)
		{
		}

		~Node()
		{
			for(std::size_t i=0;i<children.size();i++) { delete children[i]; }
		}

		bool leaf() const
		{
			return children.empty();
		}

//		void add_object(const Object& object)
//		{
//			objects.push_back(&object);
//			bounds=Box::expand_bounding_box(bounds, object);
//		}
	};

	typedef typename ListTypeProxyType<Node*>::Type Nodes;

	Node* root_;
};

#endif /* RTREE_H_ */
