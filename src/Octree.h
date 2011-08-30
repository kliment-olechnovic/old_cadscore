#ifndef OCTREE_H_
#define OCTREE_H_

#include <vector>
#include <cmath>

class Octree
{
public:
	struct Box
	{
		double x0;
		double y0;
		double z0;
		double x1;
		double y1;
		double z1;
		double x;
		double y;
		double z;

		Box(const double x0, const double y0, const double z0,
				const double x1, const double y1, const double z1) :
					x0(x0), y0(y0), z0(z0),
					x1(x1), y1(y1), z1(z1),
					x((x0+x1)/2), y((y0+y1)/2), z((z0+z1)/2)
		{
		}

		inline Box subbox(const int num) const
		{
			const int id=num%8;
			if(id==0) { return Box(x0, y0, z0, x,  y,  z ); }
			if(id==1) { return Box(x0, y0, z,  x,  y,  z1); }
			if(id==2) { return Box(x0, y,  z0, x,  y1, z ); }
			if(id==3) { return Box(x0, y,  z,  x,  y1, z1); }
			if(id==4) { return Box(x,  y0, z0, x1, y,  z ); }
			if(id==5) { return Box(x,  y0, z,  x1, y,  z1); }
			if(id==6) { return Box(x,  y,  z0, x1, y1, z ); }
			else      { return Box(x,  y,  z,  x1, y1, z1 ); }
		}

		static inline bool overlap(double a0, double a1, double b0, double b1)
		{
			if(a0>a1) std::swap(a0,a1);
			if(b0>b1) std::swap(b0,b1);
			return (fabs(a0-b0)+fabs(a1-b1)<(a1-a0)+(b1-b0));
		}

		static inline bool overlap(const Box& a, const Box& b)
		{
			return (overlap(a.x0, a.x1, b.x0, b.x1) && overlap(a.y0, a.y1, b.y0, b.y1) && overlap(a.z0, a.z1, b.z0, b.z1));
		}

		inline bool overlap(const Box& b) const
		{
			return overlap(*this, b);
		}

		inline double size() const
		{
			return (x1-x0);
		}

		template<typename SphereType>
		static inline Box from_sphere(const SphereType& s)
		{
			return Box((s.x-s.r), (s.y-s.r), (s.z-s.r), (s.x+s.r), (s.y+s.r), (s.z+s.r));
		}
	};

	Octree(const Box& bounds)
	{
		nodes_.push_back(Node(bounds));
	}

	void insert(const int id, const Box& box, const double min_allowed_box_size)
	{
		std::cout << "i start\n";
		std::deque<int> queue;
		queue.push_back(0);
		while(!queue.empty())
		{
			Node& node=nodes_[queue.back()];
			for(int j=0;j<8;j++)
			{
				std::cout << node.children[j] << "_";
			}
			std::cout << "\n";
			queue.pop_back();

			if(node.bounds.size()<min_allowed_box_size)
			{
				node.contents.push_back(id);
			}
			else
			{
				std::vector<int> clashes;
				clashes.reserve(8);
				for(int i=0;i<8;i++)
				{
					if(box.overlap(node.bounds.subbox(i)))
					{
						clashes.push_back(i);
					}
				}

				if(clashes.size()>=8)
				{
					node.contents.push_back(id);
				}
				else
				{
					for(std::size_t i=0;i<clashes.size();i++)
					{
						const int num=clashes[i];
						std::cout << num << " " << node.children[num] << " " << nodes_.size()  << " bc\n";
						for(int j=0;j<8;j++)
						{
							std::cout << node.children[j] << " ";
						}
						std::cout << " " << "\n";
						if(node.children[num]<0)
						{
							node.children[num]=nodes_.size();
							std::cout << node.children[num] << " ic1\n";
							nodes_.push_back(Node(node.bounds.subbox(num)));
							std::cout << node.children[num] << " ic2\n";
						}
						std::cout << node.children[num] << " ac\n";
						queue.push_back(node.children[num]);
					}
				}
			}
		}
		std::cout << "i end\n";
	}

	std::vector<int> intersect(const Box& box) const
	{
		std::vector<int> result;
		std::deque<int> queue;
		queue.push_back(0);
		while(!queue.empty())
		{
			const Node& node=nodes_[queue.back()];
			queue.pop_back();
			result.insert(result.end(), node.contents.begin(), node.contents.end());
			for(std::size_t i=0;i<node.children.size();i++)
			{
				const int child_id=node.children[i];
				if(child_id>=0 && box.overlap(node.bounds.subbox(i)))
				{
					queue.push_back(child_id);
				}
			}
		}
		return result;
	}

private:
	struct Node
	{
		Box bounds;
		std::vector<int> children;
		std::vector<int> contents;
		Node(const Box& bounds) : bounds(bounds), children(8, -1) {}
	};

	std::vector<Node> nodes_;
};

#endif /* OCTREE_H_ */
