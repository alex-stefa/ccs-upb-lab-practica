
#include "../KImage/KImage.h"

#ifndef __HISTOGRAM__
#define __HISTOGRAM__

class KHistogram
{
private:
	int minValue, maxValue, size;
	float* histogram;

public:
	const enum Channel { RED = 1, GREEN = 2, BLUE = 4, GRAYSCALE = 8 };

	KHistogram(CArray<int>& data);
	KHistogram(KHistogram& original);
	KHistogram(KImage& image, Channel channel = KHistogram::GRAYSCALE);

	int GetPeakValue();
	int GetPeakValue(int minValue, int maxValue);
	float GetMaxHeight();
	float GetMaxHeight(int minValue, int maxValue);
	double GetHeightsSum(float minHeight = 0);
	double GetPartialHeightsSum(float minHeight = 0);
	float GetMaxHeightForPercentage(float percentage);

	void GetSignificantBounds(float thresholdHigh, float thresholdLow, /*OUT*/ int* minBound, int* maxBound);
	void GetIntervalsAboveMinHeight(float minHeight, /*OUT*/ CArray<int>& startValues, CArray<int>& stopValues);

	KHistogram ApplyTriangleFilter(int filterWidth, float filterHeight = 1);
	KHistogram Normalize(float norm = 0);

	KImage GetBarGraph(int barWidth = 5, int barDist = 2, int maxHeight = 300, KRGBColor* color = NULL);

	~KHistogram()
	{
		if (histogram != NULL) delete[] histogram;
	}

	float GetHistogramHeight(int value)
	{
		if (value < minValue || value > maxValue) return -1;
		return histogram[value - minValue];
	}

	float* GetHistogramData()
	{
		return histogram;
	}

	int GetMinValue()
	{
		return minValue;
	}

	int GetMaxValue()
	{
		return maxValue;
	}

	int GetSize()
	{
		return size;
	}
};

#endif //__HISTOGRAM__
