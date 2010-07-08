
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


