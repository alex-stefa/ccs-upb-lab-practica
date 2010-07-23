
#include "StdAfx.h"
#include "Entity_Utils.h"
#include "Discrete_Histogram.h"
#include <set>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;


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
		return (float) hist.GetPeakValue();
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
		return (float) hist.GetPeakValue();
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

void KEntityUtils::AppendEntityArray(KEntityPointersArray& sourceEntities, /*OUT*/ KEntityPointersArray& destinationEntities)
{
	if (sourceEntities.GetSize() <= 0) return;

	int src_size = sourceEntities.GetSize();
	int dest_size = destinationEntities.GetSize();

	destinationEntities.SetSize(dest_size + src_size);

	for (int i = 0; i < src_size; ++i)
		destinationEntities[dest_size + i] = sourceEntities[i];
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

int KEntityUtils::FilterBySize(KEntityPointersArray& initialEntities, 
		/*OUT*/ KEntityPointersArray& filteredEntities, KEntityPointersArray& removedEntities,
		float minWidth, float minHeight, float maxWidth, float maxHeight)
{
	if (initialEntities.GetSize() <= 0) return 0;

	bool sameDest = (&initialEntities == &filteredEntities); // destination same as source
	ASSERT(&initialEntities != &removedEntities);
	removedEntities.RemoveAll();

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
			pFiltered->Add(pEntity);
		else
			removedEntities.Add(pEntity);
	}

	if (sameDest)
	{
		initialEntities.RemoveAll();
		initialEntities.SetSize(pFiltered->GetSize());
		for (int i = 0; i < pFiltered->GetSize(); ++i)
			initialEntities[i] = pFiltered->GetAt(i);
		delete pFiltered;
	}

	return removedEntities.GetSize();
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

/****************************************************************************************************************/

KEntityPixelMapper::KEntityPixelMapper(int width, int height) : width(width), height(height)
{
	mappedEntities = new KEntityPointersArray();
	map = new short*[height];
	for (int row = 0; row < height; ++row)
	{
		map[row] = new short[width];
		memset(map[row], unmapped, width * sizeof(short));
	}
	GetNeighbours(KEntityPixelMapper::CONNECTED_4, KEntityPixelMapper::neighbours4);
	GetNeighbours(KEntityPixelMapper::CONNECTED_8, KEntityPixelMapper::neighbours8);
}

KEntityPixelMapper::~KEntityPixelMapper()
{
	for (int i = 0; i < height; ++i) delete[] map[i];
	delete[] map;
	delete mappedEntities;
}

bool KEntityPixelMapper::IsValid(int column, int row)
{
	return column < width && column >= 0 && row < height && row >= 0;
}

void KEntityPixelMapper::AddEntity(KGenericEntity& entity, bool assertNoOverlapping)
{
	ASSERT(mappedEntities->GetSize() < (1 << 16));
	short index = mappedEntities->GetSize();
	mappedEntities->Add(&entity);

	KPropValue propValue;
	entity.GetPropertyValue(CCS_SEGMENTS, propValue);
	KEntityPointersArray* pSegments = (KEntityPointersArray*)(KPropertyInspector*) propValue;
	ASSERT(pSegments != NULL);

	for (int i = pSegments->GetUpperBound(); i >= 0; --i)
	{
		KRowSegment* pSegment = (KRowSegment*) (pSegments->GetAt(i));

		ASSERT(pSegment->intRow >= 0 && pSegment->intRow < height);
		ASSERT(pSegment->intStartColumn >= 0 && pSegment->intStartColumn < width);
		ASSERT(pSegment->intStopColumn >= 0 && pSegment->intStopColumn < width);

		for (int column = pSegment->intStartColumn; column <= pSegment->intStopColumn; ++column)
		{
			if (assertNoOverlapping && map[pSegment->intRow][column] != unmapped)
			{
				TRACE("Overlapping entities detected!");
				ASSERT(false);
			}
			map[pSegment->intRow][column] = index;
		}
	}
}

void KEntityPixelMapper::AddEntityArray(KEntityPointersArray& entities, bool assertNoOverlapping)
{
	for (int i = 0; i < entities.GetSize(); ++i)
		AddEntity(*((KGenericEntity*) entities[i]), assertNoOverlapping);
}

bool KEntityPixelMapper::RemoveEntity(KGenericEntity& entity)
{
	short index = -1;

	for (int i = 0; i < mappedEntities->GetSize(); ++i)
		if (((KGenericEntity*) mappedEntities->GetAt(i)) == &entity)
			index = (short) i;

	if (index < 0) return false;

	mappedEntities->RemoveAt(index);

	for (int row = 0; row < height; ++row)
		for (int column = 0; column < width; ++column)
		{
			if (map[row][column] == index)
				map[row][column] = unmapped;
			else if (map[row][column] > index)
				--map[row][column];
		}

	return true;
}

int KEntityPixelMapper::RemoveEntityArray(KEntityPointersArray& entities)
{
	int removed = 0;
	for (int i = 0; i < entities.GetSize(); ++i)
		if (RemoveEntity(*((KGenericEntity*) entities[i])))
			++removed;
	return removed;
}

KGenericEntity* KEntityPixelMapper::GetEntityAtPixel(int column, int row)
{
	if (column < 0 || row < 0 || column >= width || row >= height) return NULL;
	if (map[row][column] == unmapped) return NULL;
	ASSERT(map[row][column] < mappedEntities->GetSize());
	return (KGenericEntity*) mappedEntities->GetAt(map[row][column]);
}

KGenericEntity* KEntityPixelMapper::GetEntityAtPixel(CPoint& point)
{
	return GetEntityAtPixel((int) point.x, (int) point.y);
}

void KEntityPixelMapper::GetEntities(CRect& rect, /*OUT*/ KEntityPointersArray& entities, bool containedInRect)
{
	int left = min(rect.left, rect.right);
	int right = max(rect.left, rect.right);
	int top = min(rect.top, rect.bottom);
	int bottom = max(rect.top, rect.bottom);

	if (right < 0 || top < 0 || top >= height || left >= width) return;
	
	left = max(0, left);
	right = min(width, right);
	top = max(0, top);
	bottom = min(height, bottom);

	if (!containedInRect)
	{
		set<short> indices;

		for (int row = top; row <= bottom; ++row)
			for (int column = left; column <= right; ++column)
				if (map[row][column] != unmapped)
					indices.insert(map[row][column]);

		entities.RemoveAll();
		entities.SetSize(indices.size());

		int count = 0;
		for (set<short>::iterator it = indices.begin(); it != indices.end(); ++it)
		{
			ASSERT(*it >= 0 && *it < mappedEntities->GetSize());
			entities[count++] = mappedEntities->GetAt(*it);
		}
	}
	else
	{
		entities.RemoveAll();
		KPageRectangle pageRect(left, right, top, bottom);
		KGenericEntity* entity;
		for (int i = 0; i < mappedEntities->GetSize(); ++i)
		{
			entity = (KGenericEntity*) mappedEntities->GetAt(i);
			if (pageRect.Include(entity->boundingRectangle))
				entities.Add(entity);
		}
	}
}

void KEntityPixelMapper::GetEntities(/*OUT*/ KEntityPointersArray& entities)
{
	KEntityUtils::CopyEntityArray(*mappedEntities, entities);
}

int KEntityPixelMapper::GetEntityCount(CRect& rect, bool containedInRect)
{
	KEntityPointersArray* temp = new KEntityPointersArray();
	GetEntities(rect, *temp, containedInRect);
	int size = temp->GetSize();
	delete temp;
	return size;
}

int KEntityPixelMapper::GetEntityCount()
{
	return mappedEntities->GetSize();
}

void KEntityPixelMapper::GetNeighbours(VicinityMode vicinityMode, /*OUT*/ CArray<CPoint, CPoint>& neighbours)
{
	neighbours.RemoveAll();

	neighbours.Add(CPoint(+1, 0));
	neighbours.Add(CPoint(-1, 0));
	neighbours.Add(CPoint(0, -1));
	neighbours.Add(CPoint(0, +1));

	if (vicinityMode == KEntityPixelMapper::CONNECTED_8)
	{
		neighbours.Add(CPoint(-1, -1));
		neighbours.Add(CPoint(+1, +1));
		neighbours.Add(CPoint(+1, -1));
		neighbours.Add(CPoint(-1, +1));
	}
}

void KEntityPixelMapper::GetContourPoints(/*OUT*/ CArray<CPoint, CPoint>& contour, VicinityMode vicinityMode)
{
	contour.RemoveAll();

	CArray<CPoint, CPoint>& neighbours = 
		(vicinityMode == KEntityPixelMapper::CONNECTED_4) ? neighbours4 : neighbours8;
	int neighbourSize = neighbours.GetSize();

	int index, neighbourCount, nCol, nRow;

	for (int row = 0; row < height; ++row)
		for (int column = 0; column < width; ++column)
		{
			index = map[row][column];
			if (index == unmapped) continue;

			neighbourCount = 0;
			for (int nb = 0; nb < neighbourSize; ++nb)
			{
				nCol = column + neighbours[nb].x;
				nRow = row + neighbours[nb].y;
				if (IsValid(nCol, nRow) && map[nRow][nCol] == index)
					++neighbourCount;
			}

			if (neighbourCount < neighbourSize)
				contour.Add(CPoint(column, row));
		}
}

bool KEntityPixelMapper::GetContourPoints(KGenericEntity& entity, /*OUT*/ CArray<CPoint, CPoint>& contour, VicinityMode vicinityMode)
{
	contour.RemoveAll();

	short index = -1;

	for (int i = 0; i < mappedEntities->GetSize(); ++i)
		if (((KGenericEntity*) mappedEntities->GetAt(i)) == &entity)
			index = (short) i;

	if (index < 0) return false;

	KPropValue propValue;
	entity.GetPropertyValue(CCS_SEGMENTS, propValue);
	KEntityPointersArray* pSegments = (KEntityPointersArray*)(KPropertyInspector*) propValue;
	ASSERT(pSegments != NULL);

	CArray<CPoint, CPoint>& neighbours = 
		(vicinityMode == KEntityPixelMapper::CONNECTED_4) ? neighbours4 : neighbours8;
	int neighbourSize = neighbours.GetSize();

	int neighbourCount, nCol, nRow;

	for (int i = pSegments->GetUpperBound(); i >= 0; --i)
	{
		KRowSegment* pSegment = (KRowSegment*) (pSegments->GetAt(i));
		int row = pSegment->intRow;

		for (int column = pSegment->intStartColumn; column <= pSegment->intStopColumn; ++column)
		{
			ASSERT(map[row][column] == index);

			neighbourCount = 0;
			for (int nb = 0; nb < neighbourSize; ++nb)
			{
				nCol = column + neighbours[nb].x;
				nRow = row + neighbours[nb].y;
				if (IsValid(nCol, nRow) && map[nRow][nCol] == index)
					++neighbourCount;
			}

			if (neighbourCount < neighbourSize)
				contour.Add(CPoint(column, row));
		}
	}

	return true;
}

void KEntityPixelMapper::GetRowPixelCount(/*OUT*/ CArray<int, int>& rowCounts)
{
	rowCounts.RemoveAll();
	rowCounts.SetSize(height);

	for (int i = 0; i < height; ++i) rowCounts[i] = 0;

	for (int row = 0; row < height; ++row)
		for (int column = 0; column < width; ++column)
			if (map[row][column] != unmapped)
				++rowCounts[row];
}

void KEntityPixelMapper::GetColumnPixelCount(/*OUT*/ CArray<int, int>& columnCounts)
{
	columnCounts.RemoveAll();
	columnCounts.SetSize(width);

	for (int i = 0; i < width; ++i) columnCounts[i] = 0;

	for (int row = 0; row < height; ++row)
		for (int column = 0; column < width; ++column)
			if (map[row][column] != unmapped)
				++columnCounts[column];
}

void KEntityPixelMapper::GetEmptyRowIntervals(/*OUT*/ CArray<KEntityPixelMapper::Interval, KEntityPixelMapper::Interval&>& intervals)
{
	intervals.RemoveAll();


}

void KEntityPixelMapper::GetEmptyColumnIntervals(/*OUT*/ CArray<KEntityPixelMapper::Interval, KEntityPixelMapper::Interval&>& intervals)
{


}





