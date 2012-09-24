#include <iostream>
#include <sstream>
#include <fstream>

#include "protein/pdb_parsing.h"

#include "apollo/subdivided_icosahedron.h"
#include "apollo/rotation.h"

#include "auxiliaries/command_line_options.h"

void rotate_coordinates(const auxiliaries::CommandLineOptions& clo)
{
	clo.check_allowed_options("--depth: --output-prefix:");

	const std::size_t subdivision_depth=clo.isopt("--depth") ? clo.arg_in_interval<std::size_t>("--depth", 0, 2) : 0;
	const std::string output_prefix=clo.arg<std::string>("--output-prefix");

	std::vector<protein::PDBAtomRecord> records=protein::read_PDB_atom_records_from_PDB_file_stream(std::cin);

	if(!records.empty())
	{
		{
			apollo::SimplePoint center(0, 0, 0);
			for(std::size_t i=0;i<records.size();i++)
			{
				center=apollo::sum_of_points<apollo::SimplePoint>(center, records[i]);
			}
			center=center/records.size();
			for(std::size_t i=0;i<records.size();i++)
			{
				records[i].x-=center.x;
				records[i].y-=center.y;
				records[i].z-=center.z;
			}
		}

		apollo::SubdividedIcosahedron sih(subdivision_depth);
		for(std::size_t j=0;j<sih.vertices().size();j++)
		{
			const apollo::Rotation rotation=apollo::Rotation::from_two_points(apollo::SimplePoint(0, 0, 1), sih.vertices()[j]);

			std::ostringstream output_name_stream;
			output_name_stream << output_prefix << j;
			const std::string output_name=output_name_stream.str();
			std::ofstream output(output_name.c_str(), std::ios::out);

			for(std::size_t i=0;i<records.size();i++)
			{
				protein::PDBAtomRecord r=records[i];
				const apollo::SimplePoint start(r.x, r.y, r.z);
				const apollo::SimplePoint end=rotation.rotate<apollo::SimplePoint>(start);
				r.x=end.x;
				r.y=end.y;
				r.z=end.z;
				output << r.generate_PDB_file_line() << "\n";
			}
		}
	}
}
