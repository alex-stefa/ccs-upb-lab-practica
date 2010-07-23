/*! \addtogroup ImgEnt
 \{ */ 
//! \file Font_Size.cpp \brief Implementation file for KFontSize class

#include "StdAfx.h"
#include "Font_Size.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

bool KFontSize::DebugEnabled = false;
char* KFontSize::DebugOutputPath = ".\\";
char* KFontSize::FontSizeHistogramFilename = "fontsize";


//! Returns histogram of entity heights
KHistogram KFontSize::GetFontSizeHistogram(KEntityPointersArray& letters)
{
	ASSERT(letters.GetSize() != 0);

	CArray<int> heights;
	heights.SetSize(letters.GetSize());

	KGenericEntity* entity;

	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KGenericEntity*) letters[i];
		heights[i] = entity->boundingRectangle.Height();
	}

	return KHistogram(heights);
}

//! Computes peak values for low caps and high caps based on font size histogram
void KFontSize::GetFontSize(KEntityPointersArray& letters, /*OUT*/ int* lowCaps, int* highCaps)
{
	if (letters.GetSize() == 0) return;

	if (lowCaps == NULL && highCaps == NULL) return;
	KHistogram sizeHist = GetFontSizeHistogram(letters);
	if (sizeHist.GetSize() == 0) return;

	if (DebugEnabled) sizeHist.GetBarGraph().WriteImage(CString(DebugOutputPath) + CString(FontSizeHistogramFilename) + "-raw.png");
	sizeHist.ApplyTriangleFilter((int) ((float) sizeHist.GetSize() / 10 + 0.5), 1);
	if (DebugEnabled) sizeHist.GetBarGraph().WriteImage(CString(DebugOutputPath) + CString(FontSizeHistogramFilename) + "-filtered.png");

	int peakValue1 = sizeHist.GetPeakValue(sizeHist.GetMinValue(), sizeHist.GetMaxValue());
	int peakValue21 = sizeHist.GetPeakValue(sizeHist.GetMinValue(), peakValue1 - 1);
	int peakValue22 = sizeHist.GetPeakValue(peakValue1 + 1, sizeHist.GetMaxValue());

	//float peakHeight1 = sizeHist.GetHistogramHeight(peakValue1);
	//float peakHeight21 = sizeHist.GetHistogramHeight(peakValue21);
	//float peakHeight22 = sizeHist.GetHistogramHeight(peakValue22);

	if (peakValue1 - sizeHist.GetMinValue() > sizeHist.GetMaxValue() - peakValue1)
	{
		if (lowCaps != NULL) *lowCaps = peakValue21;
		if (highCaps != NULL) *highCaps = peakValue1;
	}
	else
	{
		if (lowCaps != NULL) *lowCaps = peakValue1;
		if (highCaps != NULL) *highCaps = peakValue22;
	}
}

/*! \} */
