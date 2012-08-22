#ifndef AUXILIARIES_NAME_COLORIZING_H_
#define AUXILIARIES_NAME_COLORIZING_H_

#include <map>
#include <string>

#include "color.h"

namespace auxiliaries
{

template<typename T>
class NameColorizer
{
public:
	NameColorizer()
	{
	}

	void set_map_of_colors(const std::map<T, auxiliaries::Color> map_of_colors)
	{
		map_of_colors_=map_of_colors;
	}

	const std::map<T, auxiliaries::Color>& map_of_colors() const
	{
		return map_of_colors_;
	}

	auxiliaries::Color color(const T& name) const
	{
		return color_from_map(map_of_colors_, name);
	}

	static auxiliaries::Color default_color()
	{
		return auxiliaries::Color::from_code(0xFFFFFF);
	}

private:
	static auxiliaries::Color color_from_map(const std::map<T, auxiliaries::Color>& map_of_colors, const T& name)
	{
		typename std::map<T, auxiliaries::Color>::const_iterator it=map_of_colors.find(name);
		return (it==map_of_colors.end() ? default_color() : it->second);
	}

	std::map<T, auxiliaries::Color> map_of_colors_;
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
		return color_to_string_id(color(name));
	}

	void list_colors() const
	{
		list_colors_from_map(NameColorizer<T>::map_of_colors());
	}

private:
	static std::string color_to_string_id(const auxiliaries::Color& color)
	{
		std::ostringstream output;
		output << "custom_color_" << static_cast<int>(color.r) << "_" << static_cast<int>(color.g) << "_" << static_cast<int>(color.b);
		return output.str();
	}

	static std::string color_to_string_value(const auxiliaries::Color& color)
	{
		std::ostringstream output;
		output << "[ " << color.r_double() << ", " << color.g_double() << ", " << color.b_double() << " ]";
		return output.str();
	}

	static void list_color(const auxiliaries::Color& color)
	{
		std::cout << "cmd.do('set_color " << color_to_string_id(color) << ", " << color_to_string_value(color) << "')\n";
	}

	static void list_colors_from_map(const std::map<T, auxiliaries::Color>& map_of_colors)
	{
		for(typename std::map<T, auxiliaries::Color>::const_iterator it=map_of_colors.begin();it!=map_of_colors.end();++it)
		{
			list_color(it->second);
		}
		list_color(NameColorizer<T>::default_color());
		std::cout << "\n";
	}
};

}

#endif /* AUXILIARIES_NAME_COLORIZING_H_ */
