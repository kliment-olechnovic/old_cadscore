#ifndef FLOATINGPOINTNUMBERHANDLING_H_
#define FLOATINGPOINTNUMBERHANDLING_H_

template<typename FloatingPointNumberType>
class FloatingPointNumberHandling
{
public:
	typedef FloatingPointNumberType Number;

	static bool equal(const Number a, const Number b)
	{
		return (((a-b)<epsilon()) && ((b-a)<epsilon()));
	}

	static bool less(const Number a, const Number b)
	{
		return ((a+epsilon())<b);
	}

	static bool greater(const Number a, const Number b)
	{
		return ((a-epsilon())>b);
	}

private:
	static Number epsilon()
	{
		return 0.00000001;
	}
};

#endif /* FLOATINGPOINTNUMBERHANDLING_H_ */
