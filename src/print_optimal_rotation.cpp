#include <iostream>
#include <limits>
#include <iomanip>

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

	apollo::SimplePoint max_projection_vector=sih.vertices().at(0);
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
				max_projection_vector=v;
			}
		}
	}

	double max_projection_side=-1.0;
	{
		double low_val=0.0;
		double high_val=0.0;
		for(std::size_t i=0;i<atoms.size();i++)
		{
			const double val=apollo::project_point_on_vector(center, center+max_projection_vector, atoms[i]);
			low_val=std::min(low_val, val);
			high_val=std::max(high_val, val);
		}
		if(fabs(low_val)>fabs(high_val))
		{
			max_projection_side=0.0-max_projection_side;
		}
	}

	apollo::SimplePoint min_vertical_vector=apollo::any_normal_of_vector<apollo::SimplePoint>(max_projection_vector);
	{
		const apollo::SimplePoint initial_vertical_vector=min_vertical_vector;
		const double angle_step=1.0;
		double min_height=std::numeric_limits<double>::max();
		for(double angle=angle_step;angle<360.0;angle+=angle_step)
		{
			const apollo::SimplePoint v=apollo::Rotation(max_projection_vector, angle).rotate<apollo::SimplePoint>(initial_vertical_vector);
			double low_val=0.0;
			double high_val=0.0;
			for(std::size_t i=0;i<atoms.size();i++)
			{
				const double val=apollo::project_point_on_vector(center, center+v, atoms[i]);
				low_val=std::min(low_val, val);
				high_val=std::max(high_val, val);
			}
			const double height=high_val-low_val;
			if(height<min_height)
			{
				min_height=height;
				min_vertical_vector=v;
			}
		}
	}

	const apollo::Rotation max_projection_rotation=apollo::Rotation::from_two_points(max_projection_vector, apollo::SimplePoint(0.0, 0.0, max_projection_side));

	const apollo::Rotation min_vertical_rotation=apollo::Rotation::from_two_points(max_projection_rotation.rotate<apollo::SimplePoint>(min_vertical_vector), apollo::SimplePoint(0.0, 1.0, 0.0));

	std::cout << std::fixed << "rotate [" << max_projection_rotation.axis.unit().x << "," << max_projection_rotation.axis.unit().y << "," << max_projection_rotation.axis.unit().z << "], " << max_projection_rotation.angle << "\n";

	std::cout << std::fixed << "rotate [" << min_vertical_rotation.axis.unit().x << "," << min_vertical_rotation.axis.unit().y << "," << min_vertical_rotation.axis.unit().z << "], " << min_vertical_rotation.angle << "\n";
}
