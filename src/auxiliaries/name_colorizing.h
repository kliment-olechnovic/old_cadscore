#ifndef AUXILIARIES_NAME_COLORIZING_H_
#define AUXILIARIES_NAME_COLORIZING_H_

#include <map>
#include <string>

#include "color.h"

namespace auxiliaries
{

class ColorManagementForMapping
{
public:
	static Color default_color()
	{
		return Color::from_code(0xFFFFFF);
	}

	template<typename T>
	static Color color_from_map(const std::map<T, Color>& map_of_colors, const T& name)
	{
		typename std::map<T, Color>::const_iterator it=map_of_colors.find(name);
		return (it==map_of_colors.end() ? default_color() : it->second);
	}
};

template<typename T>
class NameColorizer
{
public:
	NameColorizer()
	{
	}

	const std::map<T, Color>& map_of_colors() const
	{
		return map_of_colors_;
	}

	Color color(const T& name) const
	{
		return ColorManagementForMapping::color_from_map(map_of_colors_, name);
	}

	void set_map_of_colors(const std::map<T, Color> map_of_colors)
	{
		map_of_colors_=map_of_colors;
	}

	void add_name_color(const T& name, const Color& color)
	{
		map_of_colors_[name]=color;
	}

private:
	std::map<T, Color> map_of_colors_;
};

class ColorManagementForPymol
{
public:
	static std::string color_to_string_id(const Color& color)
	{
		std::ostringstream output;
		output << "custom_color_" << static_cast<int>(color.r) << "_" << static_cast<int>(color.g) << "_" << static_cast<int>(color.b);
		return output.str();
	}

	static std::string color_to_string_value(const Color& color)
	{
		std::ostringstream output;
		output << "[ " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << " ]";
		return output.str();
	}

	static void list_color(const Color& color)
	{
		std::cout << "cmd.set_color('" << color_to_string_id(color) << "', " << color_to_string_value(color) << ")\n";
	}

	template<typename ColorsMapType>
	static void list_colors_from_map(const ColorsMapType& map_of_colors)
	{
		for(typename ColorsMapType::const_iterator it=map_of_colors.begin();it!=map_of_colors.end();++it)
		{
			list_color(it->second);
		}
		list_color(ColorManagementForMapping::default_color());
		std::cout << "\n";
	}
};

template<typename T>
class NameColorizerForPymol : public NameColorizer<T>
{
public:
	NameColorizerForPymol()
	{
	}

	std::string color_string(const T& name) const
	{
		return ColorManagementForPymol::color_to_string_id(color(name));
	}

	void list_colors() const
	{
		ColorManagementForPymol::list_colors_from_map(NameColorizer<T>::map_of_colors());
	}
};

}

#endif /* AUXILIARIES_NAME_COLORIZING_H_ */
