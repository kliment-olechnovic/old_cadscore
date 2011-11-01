#ifndef AUXILIARIES_COLOR_H_
#define AUXILIARIES_COLOR_H_

namespace auxiliaries
{

struct Color
{
	unsigned char r;
	unsigned char g;
	unsigned char b;

	Color() : r(0), g(0), b(0)
	{
	}

	Color(const unsigned char r, const unsigned char g, const unsigned char b) : r(r), g(g), b(b)
	{
	}

	static Color from_code(const unsigned int rgb)
	{
		return Color(((rgb&0xFF0000) >> 16), ((rgb&0x00FF00) >> 8), (rgb&0x0000FF));
	}

	static Color from_temperature(const double t)
	{
		Color c;
		if(t<0)
		{
			c.b=255;
		}
		else if(t>1)
		{
			c.r=255;
		}
		else if(t<=0.5)
		{
			c.b=255;
			c.r=255*(t/0.5);
			c.g=c.r;
		}
		else if(t>0.5)
		{
			c.r=255;
			c.b=255*(1-(t-0.5)/0.5);
			c.g=c.b;
		}
		return c;
	}
};

}

#endif /* AUXILIARIES_COLOR_H_ */
