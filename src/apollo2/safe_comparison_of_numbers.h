#ifndef APOLLO2_SAFE_COMPARISON_OF_NUMBERS_H_
#define APOLLO2_SAFE_COMPARISON_OF_NUMBERS_H_

namespace apollo2
{

inline double& comparison_epsilon_reference()
{
	static double e=0.00001;
	return e;
}

inline double comparison_epsilon()
{
	return comparison_epsilon_reference();
}

inline bool equal(const double a, const double b)
{
	return (((a-b)<comparison_epsilon()) && ((b-a)<comparison_epsilon()));
}

inline bool less(const double a, const double b)
{
	return ((a+comparison_epsilon())<b);
}

inline bool greater(const double a, const double b)
{
	return ((a-comparison_epsilon())>b);
}

inline bool less_or_equal(const double a, const double b)
{
	return (less(a, b) || equal(a, b));
}

inline bool greater_or_equal(const double a, const double b)
{
	return (greater(a, b) || equal(a, b));
}

}

#endif /* APOLLO2_SAFE_COMPARISON_OF_NUMBERS_H_ */