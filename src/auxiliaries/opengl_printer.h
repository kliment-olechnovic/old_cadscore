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
		string_stream_ << obj_name_ << " = [\n";
	}

	~OpenGLPrinter()
	{
		string_stream_ << "]\ncmd.load_cgo(" << obj_name_ << ", '" << cgo_name_ << "')\n";
		std::cout << string_stream_.str() << "\n";
	}

	template<typename PointType>
	void print_tringle_fan(const std::vector<PointType>& mesh_vertices, const PointType& normal, const auxiliaries::Color& color)
	{
		if(!mesh_vertices.empty())
		{
			const PointType shift=normal*0.001;

			string_stream_ << "    BEGIN, TRIANGLE_FAN,\n";
			string_stream_ << "    COLOR, " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << ",\n";
			string_stream_ << "    NORMAL, " << point_to_string(normal) << ",\n";
			string_stream_ << "    VERTEX, " << point_to_string(mesh_vertices.back()+shift) << ",\n";
			for(std::size_t i=0;i+1<mesh_vertices.size();i++)
			{
				string_stream_ << "    NORMAL, " << point_to_string(normal) << ",\n";
				string_stream_ << "    VERTEX, " << point_to_string(mesh_vertices[i]+shift) << ",\n";
			}
			string_stream_ << "    NORMAL, " << point_to_string(normal) << ",\n";
			string_stream_ << "    VERTEX, " << point_to_string(mesh_vertices.front()+shift) << ",\n";
			string_stream_ << "    END,\n";
		}
	}

	template<typename PointType>
	void print_cylinder(const PointType& start, const PointType& end, const double radius, const auxiliaries::Color& start_color, const auxiliaries::Color& end_color)
	{
		string_stream_ << "    CYLINDER, " << point_to_string(start) << ", " << point_to_string(end) << ", " << radius << ", ";
		string_stream_ << start_color.r_double() << ", " << start_color.g_double() << ", " << start_color.b_double() << ", ";
		string_stream_ << end_color.r_double() << ", " << end_color.g_double() << ", " << end_color.b_double() << ",\n";
	}

	template<typename SphereType>
	void print_sphere(const SphereType& sphere, const auxiliaries::Color& color)
	{
		string_stream_ << "    COLOR, " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << ",\n";
		string_stream_ << "    SPHERE, " << point_to_string(sphere) << ", " << sphere.r << ",\n";
	}

	void print_alpha(const double alpha)
	{
		string_stream_ << "    ALPHA, " << alpha << ",\n";
	}

	void print_color(const auxiliaries::Color& color)
	{
		string_stream_ << "    COLOR, " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << ",\n";
	}

	template<typename PointType>
	void print_line_strip(const std::vector<PointType>& vertices)
	{
		if(!vertices.empty())
		{
			string_stream_ << "    BEGIN, LINE_STRIP,\n";
			for(std::size_t i=0;i<vertices.size();i++)
			{
				string_stream_ << "    VERTEX, " << point_to_string(vertices[i]) << ",\n";
			}
			string_stream_ << "    END,\n";
		}
	}

private:
	OpenGLPrinter(const OpenGLPrinter& /*opengl_printer*/);
	OpenGLPrinter& operator=(const OpenGLPrinter& /*opengl_printer*/);

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
	std::ostringstream string_stream_;
};

}

#endif /* AUXILIARIES_OPENGL_PRINTER_H_ */
