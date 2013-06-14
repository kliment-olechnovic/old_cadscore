#ifndef APOLLO2_OPENGL_PRINTER_H_
#define APOLLO2_OPENGL_PRINTER_H_

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <limits>

namespace apollo2
{

class OpenGLPrinter
{
public:
	OpenGLPrinter(std::ostream& output_stream, const std::string& obj_name, const std::string& cgo_name) : output_stream_(output_stream), obj_name_(obj_name), cgo_name_(cgo_name)
	{
		string_stream_ << obj_name_ << " = [\n";
	}

	~OpenGLPrinter()
	{
		string_stream_ << "]\ncmd.load_cgo(" << obj_name_ << ", '" << cgo_name_ << "')\n";
		output_stream_ << string_stream_.str() << "\n";
	}

	static void print_setup(std::ostream& output_stream)
	{
		output_stream << "from pymol.cgo import *\n";
		output_stream << "from pymol import cmd\n\n";
	}

	void print_alpha(const double alpha)
	{
		string_stream_ << "    ALPHA, " << alpha << ",\n";
	}

	void print_color(const double r, const double g, const double b)
	{
		string_stream_ << "    COLOR, " << r << ", " << g << ", " << b << ",\n";
	}

	template<typename ColorType>
	void print_color(const ColorType& color)
	{
		print_color(color.r_double(), color.g_double(), color.b_double());
	}

	template<typename SphereType>
	void print_sphere(const SphereType& sphere)
	{
		string_stream_ << "    SPHERE, " << point_to_string(sphere) << ", " << sphere.r << ",\n";
	}

	template<typename PointType, typename ColorType>
	void print_cylinder(const PointType& start, const PointType& end, const double radius, const ColorType& start_color, const ColorType& end_color)
	{
		string_stream_ << "    CYLINDER, " << point_to_string(start) << ", " << point_to_string(end) << ", " << radius << ", ";
		string_stream_ << start_color.r_double() << ", " << start_color.g_double() << ", " << start_color.b_double() << ", ";
		string_stream_ << end_color.r_double() << ", " << end_color.g_double() << ", " << end_color.b_double() << ",\n";
	}

	template<typename PointType>
	void print_tringle_fan(const std::vector<PointType>& vertices, const PointType& normal)
	{
		if(!vertices.empty())
		{
			const PointType shift=normal*0.001;

			string_stream_ << "    BEGIN, TRIANGLE_FAN,\n";
			string_stream_ << "    NORMAL, " << point_to_string(normal) << ",\n";
			string_stream_ << "    VERTEX, " << point_to_string(vertices.back()+shift) << ",\n";
			for(std::size_t i=0;i+1<vertices.size();i++)
			{
				string_stream_ << "    NORMAL, " << point_to_string(normal) << ",\n";
				string_stream_ << "    VERTEX, " << point_to_string(vertices[i]+shift) << ",\n";
			}
			string_stream_ << "    NORMAL, " << point_to_string(normal) << ",\n";
			string_stream_ << "    VERTEX, " << point_to_string(vertices.front()+shift) << ",\n";
			string_stream_ << "    END,\n";
		}
	}

	template<typename PointType>
	void print_line_strip(const std::vector<PointType>& vertices, const bool loop=false)
	{
		if(!vertices.empty())
		{
			if(loop)
			{
				string_stream_ << "    BEGIN, LINE_LOOP,\n";
			}
			else
			{
				string_stream_ << "    BEGIN, LINE_STRIP,\n";
			}
			for(std::size_t i=0;i<vertices.size();i++)
			{
				string_stream_ << "    VERTEX, " << point_to_string(vertices[i]) << ",\n";
			}
			string_stream_ << "    END,\n";
		}
	}

	template<typename PointType>
	void print_triangle_strip(const std::vector<PointType>& vertices, const std::vector<PointType>& normals)
	{
		if(!vertices.empty() && vertices.size()==normals.size())
		{
			string_stream_ << "    BEGIN, TRIANGLE_STRIP,\n";
			for(std::size_t i=0;i<vertices.size();i++)
			{
				string_stream_ << "    NORMAL, " << point_to_string(normals[i]) << ",\n";
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

	std::ostream& output_stream_;
	const std::string obj_name_;
	const std::string cgo_name_;
	std::ostringstream string_stream_;
};

}

#endif /* APOLLO2_OPENGL_PRINTER_H_ */
