#ifndef AUXILIARIES_OPENGL_PRINTER_H_
#define AUXILIARIES_OPENGL_PRINTER_H_

#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <limits>

#include "color.h"

namespace auxiliaries
{

class OpenGLPrinter
{
public:
	OpenGLPrinter(const std::string& obj_name, const std::string& cgo_name) : obj_name_(obj_name), cgo_name_(cgo_name)
	{
		std::cout << obj_name_ << " = [\n";
	}

	~OpenGLPrinter()
	{
		std::cout << "]\ncmd.load_cgo(" << obj_name_ << ", '" << cgo_name_ << "')\n\n";
	}

	template<typename PointType>
	static void print_tringle_fan(const std::vector<PointType>& mesh_vertices, const PointType& normal, const auxiliaries::Color& color)
	{
		if(!mesh_vertices.empty())
		{
			const PointType shift=normal*0.001;

			std::cout << "    BEGIN, TRIANGLE_FAN,\n";
			std::cout << "    COLOR, " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << ",\n";
			std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
			std::cout << "    VERTEX, " << point_to_string(mesh_vertices.back()+shift) << ",\n";
			for(std::size_t i=0;i+1<mesh_vertices.size();i++)
			{
				std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
				std::cout << "    VERTEX, " << point_to_string(mesh_vertices[i]+shift) << ",\n";
			}
			std::cout << "    NORMAL, " << point_to_string(normal) << ",\n";
			std::cout << "    VERTEX, " << point_to_string(mesh_vertices.front()+shift) << ",\n";
			std::cout << "    END,\n";
		}
	}

	template<typename PointType>
	static void print_cylinder(const PointType& start, const PointType& end, const double radius, const auxiliaries::Color& start_color, const auxiliaries::Color& end_color)
	{
		std::cout << "    CYLINDER, " << point_to_string(start) << ", " << point_to_string(end) << ", " << radius << ", ";
		std::cout << start_color.r_double() << ", " << start_color.g_double() << ", " << start_color.b_double() << ", ";
		std::cout << end_color.r_double() << ", " << end_color.g_double() << ", " << end_color.b_double() << ",\n";
	}

	template<typename SphereType>
	static void print_sphere(const SphereType& sphere, const auxiliaries::Color& color)
	{
		std::cout << "    COLOR, " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << ",\n";
		std::cout << "    SPHERE, " << point_to_string(sphere) << ", " << sphere.r << ",\n";
	}

private:
	template<typename PointType>
	static std::string point_to_string(const PointType& a)
	{
		std::ostringstream output;
		output.precision(std::numeric_limits<double>::digits10);
		output << std::fixed << a.x << ", " << a.y << ", " << a.z;
		return output.str();
	}

	const std::string obj_name_;
	const std::string cgo_name_;
};

}

#endif /* AUXILIARIES_OPENGL_PRINTER_H_ */
