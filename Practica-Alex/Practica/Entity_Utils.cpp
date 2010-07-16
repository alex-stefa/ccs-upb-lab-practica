
#include "StdAfx.h"
#include "Entity_Utils.h"
#include "Discrete_Histogram.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


float KEntityUtils::GetAverageWidth(KEntityPointersArray& entities, AverageMode mode)
{
	if (entities.GetSize() <= 0) return 0;

	if (mode == KEntityUtils::ARITHMETIC_MEAN)
	{
		long double sum = 0;
		for (int i = entities.GetSize()-1; i >= 0; --i)
			sum += ((KGenericEntity*) entities.GetAt(i))->boundingRectangle.Width();
		return (float) (sum / entities.GetSize());
	}

	if (mode == KEntityUtils::HISTOGRAM_PEAK)
	{
		CArray<int> sizes;
		sizes.SetSize(entities.GetSize());
		for (int i = entities.GetSize()-1; i >= 0; --i)
			sizes[i] = ((KGenericEntity*) entities.GetAt(i))->boundingRectangle.Width();
		KHistogram hist(sizes);
		return hist.GetPeakValue();
	}

	return 0;
}

float KEntityUtils::GetAverageHeight(KEntityPointersArray& entities, AverageMode mode)
{
	if (entities.GetSize() <= 0) return 0;

	if (mode == KEntityUtils::ARITHMETIC_MEAN)
	{
		long double sum = 0;
		for (int i = entities.GetSize()-1; i >= 0; --i)
			sum += ((KGenericEntity*) entities.GetAt(i))->boundingRectangle.Height();
		return (float) (sum / entities.GetSize());
	}

	if (mode == KEntityUtils::HISTOGRAM_PEAK)
	{
		CArray<int> sizes;
		sizes.SetSize(entities.GetSize());
		for (int i = entities.GetSize()-1; i >= 0; --i)
			sizes[i] = ((KGenericEntity*) entities.GetAt(i))->boundingRectangle.Height();
		KHistogram hist(sizes);
		return hist.GetPeakValue();
	}

	return 0;
}
	
void KEntityUtils::CopyEntityArray(KEntityPointersArray& initialEntities, /*OUT*/ KEntityPointersArray& copiedEntities)
{
	if (initialEntities.GetSize() <= 0) return;

	ASSERT(&initialEntities != &copiedEntities); // source and destination cannot be the same

	copiedEntities.RemoveAll();
	copiedEntities.SetSize(initialEntities.GetSize());

	for (int i = 0; i < initialEntities.GetSize(); ++i)
		copiedEntities[i] = initialEntities[i];
}

int KEntityUtils::FilterBySize(KEntityPointersArray& initialEntities, /*OUT*/ KEntityPointersArray& filteredEntities, 
		float minWidth, float minHeight, float maxWidth, float maxHeight, bool deleteFiltered)
{
	if (initialEntities.GetSize() <= 0) return 0;

	bool sameDest = (&initialEntities == &filteredEntities); // destination same as source
	int removed = 0;

	KEntityPointersArray* pFiltered = &filteredEntities;
	if (sameDest) pFiltered = new KEntityPointersArray();
	
	for (int i = 0; i < initialEntities.GetSize(); ++i)
	{
		KGenericEntity* pEntity = (KGenericEntity*) initialEntities.GetAt(i);
		int width = pEntity->boundingRectangle.Width();
		int height = pEntity->boundingRectangle.Height();

		if ((width >= minWidth) && (height >= minHeight) &&
			((maxWidth <= 0) || (width <= maxWidth)) &&
			((maxHeight <= 0) || (height <= maxHeight)))
		{
			pFiltered->Add(pEntity);
		}
		else
		{
			if (sameDest && deleteFiltered) delete pEntity;
			++removed;
		}
	}

	if (sameDest)
	{
		initialEntities.RemoveAll();
		initialEntities.SetSize(pFiltered->GetSize());
		for (int i = 0; i < pFiltered->GetSize(); ++i)
			initialEntities[i] = pFiltered->GetAt(i);
		delete pFiltered;
	}

	return removed;
}

void KEntityUtils::GenerateEntityContours(KGenericEntity& entity, /*OUT*/ KPointSet& exteriorPoints, KPointSet& interiorPoints)
{
	KPageRectangle& BoundingRectangle = entity.boundingRectangle;

	KPropValue val;
	entity.GetPropertyValue(CCS_SEGMENTS, val);
	KEntityPointersArray* Segments = (KEntityPointersArray*) (KPropertyInspector*) val;
	ASSERT(Segments != NULL);

	exteriorPoints.RemoveAll();
	exteriorPoints.SetSize(0, 0x100);
	interiorPoints.RemoveAll();
	interiorPoints.SetSize(0, 0x100);

	int intLastSegment = Segments->GetUpperBound();
	ASSERT(intLastSegment >= 0);
	if (intLastSegment < 0) return;

	KPageCoordinate *intLastY = new KPageCoordinate[BoundingRectangle.right + 1];
	KPageCoordinate *intLastA = new KPageCoordinate[BoundingRectangle.right + 1];
	KPageCoordinate *intLastV = new KPageCoordinate[BoundingRectangle.right + 1];
	memset(intLastY, 0xFF, (BoundingRectangle.right + 1) * sizeof(KPageCoordinate));
	memset(intLastA, 0xFF, (BoundingRectangle.right + 1) * sizeof(KPageCoordinate));

	KRowSegment *pRowSegment;
	KPageCoordinate intStartColumn, intStopColumn, intX, intY;
	int intSegment;

	for (intSegment = 0; intSegment <= intLastSegment; intSegment++)
	{
		pRowSegment = (KRowSegment*) Segments->GetAt(intSegment);
		intY = KPageCoordinate(pRowSegment->intRow);
		intStartColumn = KPageCoordinate(pRowSegment->intStartColumn);
		intStopColumn = KPageCoordinate(pRowSegment->intStopColumn);
		for (intX = intStartColumn; intX <= intStopColumn; intX++)
			intLastV[intX] = intY;
	}

	for (intSegment = 0; intSegment <= intLastSegment; intSegment++)
	{
		pRowSegment = (KRowSegment*) Segments->GetAt(intSegment);
		intY = KPageCoordinate(pRowSegment->intRow);
		intStartColumn = KPageCoordinate(pRowSegment->intStartColumn);
		intStopColumn = KPageCoordinate(pRowSegment->intStopColumn);

		if (intLastY[intStartColumn] != intLastA[intStartColumn] && intLastY[intStartColumn] < intY - 1)
			interiorPoints.AddPoint(intStartColumn, intLastY[intStartColumn]);

		if (intSegment == 0 || 
			KPageCoordinate(((KRowSegment*) Segments->GetAt(intSegment - 1))->intRow) != intY || 
			intLastY[intStartColumn] < 0 || 
			intLastV[intStartColumn] == intY)
			exteriorPoints.AddPoint(intStartColumn, intY);
		else
			interiorPoints.AddPoint(intStartColumn, intY);
		intLastY[intStartColumn] = intLastA[intStartColumn] = intY;

		if (intStartColumn != intStopColumn)
		{
			if (intLastY[intStopColumn] != intLastA[intStopColumn] && 
				intLastY[intStopColumn] < intY - 1)
				interiorPoints.AddPoint(intStopColumn, intLastY[intStopColumn]);

			if (intSegment == intLastSegment || 
				KPageCoordinate(((KRowSegment*) Segments->GetAt(intSegment + 1))->intRow) != intY || 
				intLastY[intStopColumn] < 0 ||
				intLastV[intStopColumn] == intY)
				exteriorPoints.AddPoint(intStopColumn, intY);
			else
				interiorPoints.AddPoint(intStopColumn, intY);
			intLastY[intStopColumn] = intLastA[intStopColumn] = intY;
		}
		else
			continue;

		for (intX = KPageCoordinate(intStartColumn + 1); intX < intStopColumn; intX++)
		{
			if (intLastY[intX] < 0)
			{
				exteriorPoints.AddPoint(intX, intY);
				intLastA[intX] = intY;
			}
			else
				if (intY - intLastY[intX] > 1)
				{
					if (intLastY[intX] != intLastA[intX])
						interiorPoints.AddPoint(intX, intLastY[intX]);
					interiorPoints.AddPoint(intX, intY);
					intLastA[intX] = intY;
				}
			intLastY[intX] = intY;
		}
	}

	for (intX = BoundingRectangle.left; intX <= BoundingRectangle.right; intX++)
		if (intLastY[intX] != intLastA[intX])
			exteriorPoints.AddPoint(intX, intLastY[intX]);  

	delete [] intLastY;
	delete [] intLastA;
	delete [] intLastV;
}

