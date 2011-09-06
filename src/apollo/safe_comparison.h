#ifndef SAFE_COMPARISON_H_
#define SAFE_COMPARISON_H_

namespace apollo
{

inline double epsilon()
{
	return 0.000001;
}

inline bool equal(const double a, const double b)
{
	return (((a-b)<epsilon()) && ((b-a)<epsilon()));
}

inline bool less(const double a, const double b)
{
	return ((a+epsilon())<b);
}

inline bool greater(const double a, const double b)
{
	return ((a-epsilon())>b);
}

}

#endif /* SAFE_COMPARISON_H_ */
