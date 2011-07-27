#ifndef FLOATINGPOINTHANDLER_H_
#define FLOATINGPOINTHANDLER_H_

class FloatingPointHandler
{
public:
	static double epsilon()
	{
		return 0.00000001;
	}

	static bool equal(double a, double b)
	{
		return (((a-b)<epsilon()) && ((b-a)<epsilon()));
	}

	static bool less(double a, double b)
	{
		return ((a+epsilon())<b);
	}

	static bool greater(double a, double b)
	{
		return ((a-epsilon())>b);
	}
};

typedef FloatingPointHandler fph;

#endif /* FLOATINGPOINTHANDLER_H_ */
