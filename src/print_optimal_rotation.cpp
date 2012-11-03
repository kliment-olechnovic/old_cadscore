#include <iostream>

#include "protein/atom.h"

#include "apollo/points_basic_operations.h"
#include "apollo/subdivided_icosahedron.h"
#include "apollo/rotation.h"

#include "auxiliaries/command_line_options.h"
#include "auxiliaries/vector_io.h"

void print_optimal_rotation(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--depth:");

	const std::size_t subdivision_depth=clo.isopt("--depth") ? clo.arg_in_interval<std::size_t>("--depth", 0, 3) : 2;

	const std::vector<protein::Atom> atoms=auxiliaries::read_vector<protein::Atom>(std::cin, "atoms", "atoms", false);

	const apollo::SubdividedIcosahedron sih(subdivision_depth);

	apollo::SimplePoint center;
	for(std::size_t i=0;i<atoms.size();i++)
	{
		center=apollo::sum_of_points<apollo::SimplePoint>(center, atoms[i]);
	}
	center=center/static_cast<double>(atoms.size());

	apollo::SimplePoint max_v=sih.vertices().at(0);
	{
		double max_sum=0.0;
		for(std::size_t j=1;j<sih.vertices().size();j++)
		{
			const apollo::SimplePoint& v=sih.vertices()[j];
			double sum=0.0;
			for(std::size_t i=0;i<atoms.size();i++)
			{
				sum+=apollo::distance_from_point_to_line(center, center+v, atoms[i]);
			}
			if(sum>max_sum)
			{
				max_sum=sum;
				max_v=v;
			}
		}
	}

	double side=1.0;
	{
		double vertical_sum=0;
		{
			double low_val=0.0;
			double high_val=0.0;
			for(std::size_t i=0;i<atoms.size();i++)
			{
				const double val=apollo::project_point_on_vector(center, center+max_v, atoms[i]);
				low_val=std::min(low_val, val);
				high_val=std::max(high_val, val);
			}
			const apollo::SimplePoint median_center=center+(max_v*((low_val+high_val)/2));
			for(std::size_t i=0;i<atoms.size();i++)
			{
				vertical_sum+=apollo::project_point_on_vector(median_center, median_center+max_v, atoms[i]);
			}
		}
		if(vertical_sum<0.0)
		{
			side=-1.0;
		}
	}

	const apollo::Rotation rotation=apollo::Rotation::from_two_points(max_v, apollo::SimplePoint(0, 0, side));

	std::cout << "rotate [" << rotation.axis.x << "," << rotation.axis.y << "," << rotation.axis.z << "], " << rotation.angle << "\n";
}
