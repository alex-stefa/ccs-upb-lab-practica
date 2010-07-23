/*! \addtogroup ImgEnt
 \{ */ 
//! \file Boldness.cpp \brief Implementation file for KBoldness class

#include "StdAfx.h"
#include "Boldness.h"
#include "Discrete_Histogram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define BLACK 0
#define WHITE 255

bool KBoldness::DebugEnabled = false;
char* KBoldness::DebugOutputPath = ".\\";
char* KBoldness::CrosshairHistogramFilename = "crosshair";


//! Sets black pixels in pixel matrix corresponding to entity (assumes entity fits inside allocated matrix)
static void SetBlackPixels(KGenericEntity& entity, BYTE** pixels, int maxWidth, int maxHeight)
{
	ASSERT(entity.boundingRectangle.Width() <= maxWidth);
	ASSERT(entity.boundingRectangle.Height() <= maxHeight);

	for (int i = 0; i < maxHeight; ++i)
		memset(pixels[i], WHITE, maxWidth * sizeof(BYTE));

	KPropValue propValue;
	entity.GetPropertyValue(CCS_SEGMENTS, propValue);
	KEntityPointersArray* pSegments = (KEntityPointersArray*)(KPropertyInspector*) propValue;
	ASSERT(pSegments != NULL);

	KRowSegment* segment;
	int row, start, stop;

	for (int i = pSegments->GetUpperBound(); i >= 0; --i)
	{
		KRowSegment* segment = (KRowSegment*) (pSegments->GetAt(i));
		row = segment->intRow - entity.boundingRectangle.top;
		start = segment->intStartColumn - entity.boundingRectangle.left;
		stop = segment->intStopColumn - entity.boundingRectangle.left;
		for (int column = start; column <= stop; ++column)
			pixels[row][column] = BLACK;
	}
}


//! Gets number of contour points in a entity specified by pixel matrix (= number of black pixels having at least a white pixel neighbour)
static int CountContourPoints(BYTE** pixels, int width, int height)
{
	int pixelCount = 0;

	for (int row = 0; row < height; ++row)
		for (int column = 0; column < width; ++column)
			if (pixels[row][column] == BLACK &&
				((row == 0 || pixels[row-1][column] == WHITE) ||
				(row == height-1 || pixels[row+1][column] == WHITE) ||
				(column == 0 || pixels[row][column-1] == WHITE) ||
				(column == width-1 || pixels[row][column+1] == WHITE)))
				++pixelCount;

	return pixelCount;
}


//! Returns average fill ratio (of black pixels) of given entities
double KBoldness::BlackPercentage(KEntityPointersArray& letters)
{
	if (letters.GetSize() <= 0) return 0;

	double avgBlack = 0;
	double totalPixels = 0;
	int nrPixels;
	KGenericEntity* entity;
	KPropValue val;

	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KGenericEntity*) letters[i];
		entity->GetPropertyValue(CCS_NUMBER_OF_PIXELS, val);
		nrPixels = (int) val;
		avgBlack += (double) nrPixels * nrPixels / entity->boundingRectangle.Area();
		totalPixels += nrPixels;
	}

	return avgBlack / totalPixels;
}


//! Returns the average ratio of the entity contour length divided by the number of black pixels inside the contour
double KBoldness::ContourLength(KEntityPointersArray &letters)
{
	if (letters.GetSize() <= 0) return 0;

	int maxWidth = -1;
	int maxHeight = -1;

	KGenericEntity* entity;
	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KGenericEntity*) letters[i];
		if (maxWidth < entity->boundingRectangle.Width()) maxWidth = entity->boundingRectangle.Width();
		if (maxHeight < entity->boundingRectangle.Height()) maxHeight = entity->boundingRectangle.Height();
	}

	BYTE** pixels = new BYTE*[maxHeight];
	for (int i = 0; i < maxHeight; ++i) pixels[i] = new BYTE[maxWidth];

	double avgContour = 0;

	KPropValue val;
	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KGenericEntity*) letters[i];
		entity->GetPropertyValue(CCS_NUMBER_OF_PIXELS, val);
		SetBlackPixels(*entity, pixels, maxWidth, maxHeight);
		avgContour += (double) CountContourPoints(pixels, 
			entity->boundingRectangle.Width(), entity->boundingRectangle.Height()) / (int) val;
	}

	for (int i = 0; i < maxHeight; ++i) delete[] pixels[i];
	delete[] pixels;

	return avgContour / letters.GetSize();
}


//! Returns average of shortest segment for given entities
// For each entity, the minium lenght of maximum segments on each row will be selected.
double KBoldness::ShortestSegment(KEntityPointersArray& letters)
{
	if (letters.GetSize() <= 0) return 0;

	double avgLength = 0;
	int entityCount = 0;
	int currLength, minLength, currRow, maxRowLength;

	KGenericEntity* entity;
	KEntityPointersArray* pSegments;
	KRowSegment* segment;
	KPropValue propValue;
	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KEntity*) letters[i];

		minLength = currRow = maxRowLength = -1;

		entity->GetPropertyValue(CCS_SEGMENTS, propValue);
		pSegments = (KEntityPointersArray*)(KPropertyInspector*) propValue;
		ASSERT(pSegments != NULL);

		for (int i = pSegments->GetUpperBound(); i >= 0; --i)
		{
			segment = (KRowSegment*) (pSegments->GetAt(i));

			if (segment->intRow == entity->boundingRectangle.top ||
				segment->intRow == entity->boundingRectangle.bottom) 
				continue;

			currLength = abs(segment->intStopColumn - segment->intStartColumn) + 1;
			
			if (segment->intRow != currRow)
			{
				if (maxRowLength < minLength || minLength < 0) minLength = maxRowLength;
				currRow = segment->intRow;
				maxRowLength = currLength;
			}
			else if (maxRowLength < currLength)
				maxRowLength = currLength;
		}

		if (maxRowLength < minLength || minLength < 0) minLength = maxRowLength;

		if (minLength > 0)
		{
			avgLength += minLength;
			++entityCount;
		}
	}

	return avgLength / entityCount;
}


//! Returns average of max pen width size for given entities
double KBoldness::MaxPenWidth(KEntityPointersArray& letters)
{
	if (letters.GetSize() <= 0) return 0;
	
	double avgPen = 0;
	KGenericEntity* entity;
	KPropValue val;

	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KGenericEntity*) letters[i];
		entity->GetPropertyValue(CCS_MAX_PEN_WIDTH, val);
		avgPen += (int) val;
	}

	return avgPen / letters.GetSize();
}


//! Returns average of avg pen width size for given entities
double KBoldness::AvgPenWidth(KEntityPointersArray& letters)
{
	if (letters.GetSize() <= 0) return 0;
	
	double avgPen = 0;
	KGenericEntity* entity;
	KPropValue val;

	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KGenericEntity*) letters[i];
		entity->GetPropertyValue(CCS_AVERAGE_PEN_WIDTH, val);
		avgPen += (int) val;
	}

	return avgPen / letters.GetSize();
}


//! Returns average pen crosshair size for given entities
double KBoldness::DomCrosshairSize(KEntityPointersArray& letters)
{
	if (letters.GetSize() <= 0) return 0;
	
	KGenericEntity* entity;
	KRowSegment* vSegment;
	KRowSegment* hSegment;
	int hLen, vLen;

	CArray<int> sizes;

	for (int i = 0; i < letters.GetSize(); ++i)
	{
		entity = (KGenericEntity*) letters[i];

		KPropValue propValue;
		entity->GetPropertyValue(CCS_SEGMENTS, propValue);
		KEntityPointersArray* pSegments = (KEntityPointersArray*)(KPropertyInspector*) propValue;
		ASSERT(pSegments != NULL);

		KRowSegments vertSegments;
		KSegmentOperations::GenerateReverseSegments(entity->boundingRectangle, *pSegments, vertSegments); 

		for (int i = pSegments->GetUpperBound(); i >= 0; --i)
		{
			hSegment = (KRowSegment*) pSegments->GetAt(i);
			hLen = abs(hSegment->intStopColumn - hSegment->intStartColumn) + 1;

			for (int j = vertSegments.GetSize()-1; j>= 0; --j)
			{
				vSegment = &vertSegments[j];
				vLen = abs(vSegment->intStopColumn - vSegment->intStartColumn) + 1;

				//intersection condition
				if (hSegment->intRow >= vSegment->intStartColumn && hSegment->intRow <= vSegment->intStopColumn
					&& vSegment->intRow >= hSegment->intStartColumn && vSegment->intRow <= hSegment->intStopColumn) 
					sizes.Add(min(hLen, vLen));
			}
		}
	}

	KHistogram hist(sizes);

	if (DebugEnabled) hist.GetBarGraph().WriteImage(CString(DebugOutputPath) + CString(CrosshairHistogramFilename) + ".png");

	return hist.GetPeakValue();
}
/*! \} */