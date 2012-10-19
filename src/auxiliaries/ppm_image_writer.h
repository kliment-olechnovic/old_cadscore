#ifndef AUXILIARIES_PPM_IMAGE_WRITER_H_
#define AUXILIARIES_PPM_IMAGE_WRITER_H_

#include <iostream>
#include <vector>

#include "color.h"

namespace auxiliaries
{

class PPMImageWriter
{
public:
	PPMImageWriter(const std::size_t height, const std::size_t width) :
		height_(height),
		width_(width),
		pixels_(height, std::vector<Color>(width))
	{
	}

	std::size_t height()
	{
		return height_;
	}

	std::size_t width()
	{
		return width_;
	}

	Color& color(const std::size_t i, const std::size_t j)
	{
		return pixels_.at(i).at(j);
	}

	const Color& const_color(const std::size_t i, const std::size_t j) const
	{
		return pixels_.at(i).at(j);
	}

	void write(std::ostream& output) const
	{
		output << "P6\n" << width_ << " " << height_ << "\n255\n";
		const std::size_t line_size=width_*3;
		char* line=new char[line_size];
		for(std::size_t i=0;i<height_;i++)
		{
			for(std::size_t j=0;j<width_;j++)
			{
				const Color& c=const_color(i, j);
				line[j*3+0]=static_cast<char>(c.r);
				line[j*3+1]=static_cast<char>(c.g);
				line[j*3+2]=static_cast<char>(c.b);
			}
			output.write(line, line_size);
		}
		delete[] line;
	}

private:
	PPMImageWriter(const PPMImageWriter&)
	{
	}

	std::size_t height_;
	std::size_t width_;
	std::vector< std::vector<Color> > pixels_;
};

}

#endif /* AUXILIARIES_PPM_IMAGE_WRITER_H_ */
