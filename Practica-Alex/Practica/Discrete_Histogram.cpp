
#include "StdAfx.h"
#include "Discrete_Histogram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MIRROR(index) (((index) < 0) ? (-(index)) : (((index) >= size) ? (2*size-(index)-2) : (index)))


KHistogram::KHistogram(CArray<int> &data)
{
	if (data.GetSize() == 0)
	{
		minValue = size = 0;
		maxValue = -1;
		histogram = NULL;
		return;
	}

	maxValue = minValue = data[0];

	for (int i = 1; i < data.GetSize(); ++i)
	{
		if (data[i] > maxValue) maxValue = data[i];
		if (data[i] < minValue) minValue = data[i];
	}

	size = maxValue - minValue + 1;
	histogram = new float[size];
	memset(histogram, 0, size * sizeof(float));

	for (int i = 0; i < data.GetSize(); ++i)
		histogram[data[i] - minValue]++;
}


void ValidateImage(KImage& image, KHistogram::Channel channel)
{
	if (!image.IsValid())
	{
		TRACE("\n Image is not valid!\n");
		ASSERT(false);
	}
	if (image.IsBitonal())
	{
		TRACE("\n Image is bilevel!\n");
		ASSERT(false);
	}
	if (!image.IsHighColor() && channel != KHistogram::GRAYSCALE)
	{
		TRACE("\n Color analysis requested for grayscale image!\n");
		ASSERT(false);
	}
}


KHistogram::KHistogram(KImage& image, Channel channel)
{
	minValue = size = 0;
	maxValue = -1;
	histogram = NULL;

	ValidateImage(image, channel);

	minValue = 0;
	maxValue = 255;
	size = 256;
	histogram = new float[size];
	memset(histogram, 0, size * sizeof(float));

	int height = image.GetPixelHeight();
	int width = image.GetPixelWidth();

	image.BeginDirectAccess(false);
	if (image.IsHighColor() && channel == KHistogram::GRAYSCALE)
	{
		for (int row = 0; row < height; ++row)
		{
			BYTE* lineData = image.GetLinePtr(row);
			for (int column = 0; column < width; ++column)
				histogram[(BYTE) (0.3 * lineData[column * 3] + 0.59 * lineData[column * 3 + 1] + 0.11 * lineData[column * 3 + 2])]++;
		}
	}
	else
	{
		for (int row = 0; row < height; ++row)
		{
			BYTE* lineData = image.GetLinePtr(row);
			for (int column = 0; column < width; ++column)
			{
				int index = column;
				if (channel == KHistogram::RED) index = column * 3;
				if (channel == KHistogram::GREEN) index = column * 3 + 1;
				if (channel == KHistogram::BLUE) index = column * 3 + 2;
				histogram[lineData[index]]++;
			}
		}
	}
	image.EndDirectAccess();
}


KHistogram::KHistogram(KHistogram& original)
{
	minValue = original.minValue;
	maxValue = original.maxValue;
	size = original.size;
	histogram = NULL;
	if (size >= 0) histogram = new float[size];
	for (int i = 0; i < size; ++i)
		histogram[i] = original.histogram[i];
}


int KHistogram::GetPeakValue()
{
	if (size <= 1) return maxValue;

	float vMax = histogram[0] + histogram[1];
	float val;
	int index = -1;

	for (int i = 1; i < size; ++i)
	{
		val = histogram[i-1] * 0.5 + histogram[i] + histogram[MIRROR(i+1)] * 0.5;
		if (val > vMax)
		{
			vMax = val;
			index = i;
		}
	}

	return index + minValue;
}


int KHistogram::GetPeakValue(int leftValue, int rightValue)
{
	if (rightValue < minValue) return -1;
	if (leftValue > maxValue) return -1;

	leftValue = min(max(leftValue, minValue), maxValue);
	rightValue = max(min(rightValue, maxValue), minValue);
	
	int maxIndex = -1;
	float vMax = -1;

	for (int i = leftValue; i <= rightValue; ++i)
		if (vMax < histogram[i - minValue] || vMax < 0)
		{
			vMax = histogram[i - minValue];
			maxIndex = i;
		}

	return maxIndex;
}


float KHistogram::GetMaxHeight()
{
	if (size == 0) return -1;

	float vMax = histogram[0];

	for (int i = 1; i < size; ++i)
		if (vMax < histogram[i])
			vMax = histogram[i];

	return vMax;
}


float KHistogram::GetMaxHeight(int leftValue, int rightValue)
{
	return GetHistogramHeight(GetPeakValue(leftValue, rightValue));
}


KHistogram KHistogram::Normalize(float norm)
{
	KHistogram new_histogram(*this);
	if (norm == 0) norm = GetMaxHeight();
	for (int i = 0; i < size; ++i) new_histogram.histogram[i] /= norm;
	return new_histogram;
}


KImage KHistogram::GetBarGraph(int barWidth, int barDist, int maxHeight, KRGBColor* color)
{
	int padding = 2;
	float vMax = GetMaxHeight();
	KRGBColor black(0,0,0);
	KRGBColor RGBColor;

	KImage img(CSize(2 * padding + size * (barWidth + barDist) - barDist, 2 * padding + maxHeight), 24);
	img.Invert();

	img.BeginDirectAccess(true);

	for (int i = 0; i < size; ++i)
	{
		if (color == NULL)
		{
			RGBColor.r = BYTE(double(rand() * 0x80) / RAND_MAX + 0.5) + 0x40;
			RGBColor.g = BYTE(double(rand() * 0x80) / RAND_MAX + 0.5) + 0x40;
			RGBColor.b = BYTE(double(rand() * 0x80) / RAND_MAX + 0.5) + 0x40;
		}
		else
			RGBColor = *color;

		for (int w = 0; w < barWidth; ++w)
			for (int h = (int)(maxHeight * histogram[i] / vMax) - 1; h >= 0; --h)
				img.Put24BPPPixel(padding + i * (barWidth + barDist) + w, maxHeight + padding - h, &RGBColor);

		if (histogram[i] == vMax)
			for (int h = 0; h < maxHeight; ++h)
				img.Put24BPPPixel(padding + i * (barWidth + barDist) + barWidth/2, maxHeight + padding - h, &black);
	}

	for (int i = 0; i < img.GetPixelWidth(); ++i)
		img.Put24BPPPixel(i, maxHeight + padding, &black);

	img.EndDirectAccess();

	return img;
}


KHistogram KHistogram::ApplyTriangleFilter(int filterWidth, float filterHeight)
{
	float t_sum, t_weight, c_weight;

	KHistogram new_histogram(*this);

	for (int center = 0; center < size; ++center)
	{
		t_sum = t_weight = 0;

		for (int position = -filterWidth; position <= +filterWidth; ++position)
		{
			c_weight = (filterWidth - abs(position)) * filterHeight / filterWidth;
			t_weight += c_weight;
			t_sum += c_weight * new_histogram.histogram[MIRROR(center + position)];
		}

		new_histogram.histogram[center] = t_sum / t_weight;
	}

	return new_histogram;
}


void KHistogram::GetSignificantBounds(float thresholdHigh, float thresholdLow, /*OUT*/ int* minBound, int* maxBound)
{
	ASSERT(thresholdHigh >= thresholdLow);
	ASSERT(thresholdHigh >= 0 && thresholdLow >= 0);
	if (minBound == NULL && maxBound == NULL) return;
	
	if (thresholdHigh == 0)
	{
		if (minBound != NULL) *minBound = GetMinValue();
		if (maxBound != NULL) *maxBound = GetMaxValue();
		return;
	}

	int bestStart, bestStop, lastStart, lastStop;

	lastStart = bestStart = 0;
	lastStop = bestStop = -1;
	while (lastStart < size)
	{
		while (histogram[lastStart] < thresholdHigh && lastStart < size) ++lastStart;
		lastStop = lastStart;
		while (histogram[lastStop] >= thresholdHigh && lastStop < size) ++lastStop;
		if (lastStop - lastStart > bestStop - bestStart + 1)
		{
			bestStart = lastStart;
			bestStop = lastStop - 1;
		}
		lastStart = lastStop;
	}

	if (bestStop < 0)
	{
		if (minBound != NULL) *minBound = -1;
		if (maxBound != NULL) *maxBound = -1;
		return;
	}

	while (bestStart >= 0 && histogram[bestStart] >= thresholdLow) --bestStart;
	while (bestStop < size && histogram[bestStop] >= thresholdLow) ++bestStop;

	if (minBound != NULL) *minBound = bestStart + 1;
	if (maxBound != NULL) *maxBound = bestStop - 1;
}


double KHistogram::GetHeightsSum(float minHeight)
{
	double result = 0;
	for (int i = 0; i < size; ++i)
		if (histogram[i] >= minHeight)
			result += histogram[i];
	return result;
}

double KHistogram::GetPartialHeightsSum(float minHeight)
{
	double result = 0;
	for (int i = 0; i < size; ++i)
		if (histogram[i] >= minHeight)
			result += histogram[i] - minHeight;
	return result;
}

//! really slow!
float KHistogram::GetMaxHeightForPercentage(float percentage)
{
	if (percentage <= 0) return GetMaxHeight();
	if (percentage >= 1) return 0;
	if (size <= 0) return -1;

	double totalSum = GetHeightsSum();
	double targetSum = totalSum * (1 - percentage);

	float currentHeight = totalSum * percentage / size;

	while (GetPartialHeightsSum(currentHeight) > targetSum) ++currentHeight;

	return currentHeight;
}

void KHistogram::GetIntervalsAboveMinHeight(float minHeight, /*OUT*/ CArray<int>& startValues, CArray<int>& stopValues)
{
	if (size <= 0) return;

	int start = 0, stop;

	while (start < size)
	{
		while (start < size && histogram[start] < minHeight) ++start;
		if (start >= size) break;
		stop = start;
		while (stop < size && histogram[stop] >= minHeight) ++stop;
		ASSERT(start <= stop - 1);
		startValues.Add(minValue + start);
		stopValues.Add(minValue + stop - 1);
		start = stop;
	}
}
