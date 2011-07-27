#ifndef FLOATINGPOINTNUMBERHANDLING_H_
#define FLOATINGPOINTNUMBERHANDLING_H_

template<typename FloatingPointNumberType>
class FloatingPointNumberHandling
{
public:
	typedef FloatingPointNumberType FloatType;

	static bool equal(const FloatType a, const FloatType b)
	{
		return (((a-b)<epsilon()) && ((b-a)<epsilon()));
	}

	static bool less(const FloatType a, const FloatType b)
	{
		return ((a+epsilon())<b);
	}

	static bool greater(const FloatType a, const FloatType b)
	{
		return ((a-epsilon())>b);
	}

private:
	static FloatType epsilon()
	{
		return 0.00000001;
	}
};

#endif /* FLOATINGPOINTNUMBERHANDLING_H_ */
