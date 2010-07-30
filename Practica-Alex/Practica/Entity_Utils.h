

#pragma once

#include "Discrete_Histogram.h"
#include <set>


class KEntityUtils
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	struct Interval
	{
		int start, stop;
		Interval() : start(0), stop(0) {}
		Interval(int start, int stop) : start(start), stop(stop) {}

		inline bool Includes(int begin, int end) { return (start <= begin) && (end <= stop); }
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	const enum AverageMode { ARITHMETIC_MEAN = 1, HISTOGRAM_PEAK = 2 };
	static float GetAverageWidth(KEntityPointersArray& entities, AverageMode mode = KEntityUtils::HISTOGRAM_PEAK);
	static float GetAverageHeight(KEntityPointersArray& entities, AverageMode mode = KEntityUtils::HISTOGRAM_PEAK);
	
	static int FilterBySize(KEntityPointersArray& initialEntities, 
		/*OUT*/ KEntityPointersArray& filteredEntities, 
		float minWidth = 0, float minHeight = 0, float maxWidth = 0, float maxHeight = 0, 
		bool deleteFiltered = false);
	static int FilterBySize(KEntityPointersArray& initialEntities, 
		/*OUT*/ KEntityPointersArray& filteredEntities, KEntityPointersArray& removedEntities,
		float minWidth = 0, float minHeight = 0, float maxWidth = 0, float maxHeight = 0);
	
	static void CopyEntityArray(KEntityPointersArray& initialEntities, 
		/*OUT*/ KEntityPointersArray& copiedEntities);
	static void AppendEntityArray(KEntityPointersArray& sourceEntities, 
		/*OUT*/ KEntityPointersArray& destinationEntities);
	
	static void GenerateEntityContours(KGenericEntity& entity, 
		/*OUT*/ KPointSet& exteriorPoints, KPointSet& interiorPoints);

	static void GetOccupiedRowIntervals(KEntityPointersArray& entities, 
		/*OUT*/ CArray<Interval, Interval&>& intervals);
	static void GetOccupiedColumnIntervals(KEntityPointersArray& entities, 
		/*OUT*/ CArray<Interval, Interval&>& intervals);
	static void GetComplementaryIntervals(CArray<Interval, Interval&>& origIntervals, int minLimit, int maxLimit,
		/*OUT*/ CArray<Interval, Interval&>& compIntervals);

	static void GroupByRowIntervals(KEntityPointersArray& entities, CArray<Interval, Interval&>& intervals,
		/*OUT*/ CArray<KEntityPointersArray, KEntityPointersArray&>& groups);
	static void GroupByColumnIntervals(KEntityPointersArray& entities, CArray<Interval, Interval&>& intervals,
		/*OUT*/ CArray<KEntityPointersArray, KEntityPointersArray&>& groups);

};

/*****************************************************************************************************************/
/*****************************************************************************************************************/

class KEntityPixelMapper
{
public:
	KEntityPixelMapper(int width, int height);

	inline int GetWidth() { return width; }
	inline int GetHeight() { return height; }

	void AddEntity(KGenericEntity& entity, bool assertNoOverlapping = true);
	void AddEntityArray(KEntityPointersArray& entities, bool assertNoOverlapping = true);

	bool RemoveEntity(KGenericEntity& entity);
	int RemoveEntityArray(KEntityPointersArray& entities);

	KGenericEntity* GetEntityAtPixel(int column, int row);
	KGenericEntity* GetEntityAtPixel(CPoint& point);

	void GetEntities(/*OUT*/ KEntityPointersArray& entities);
	void GetEntities(CRect& rect, /*OUT*/ KEntityPointersArray& entities, bool containedInRect = false);
	int GetEntityCount();
	int GetEntityCount(CRect& rect, bool containedInRect = false);

	inline int GetIndexAtPixel(int column, int row) { return map[row][column]; }

	const enum VicinityMode { CONNECTED_4 = 1, CONNECTED_8 = 2 };
	void GetContourPoints(/*OUT*/ CArray<CPoint, CPoint>& contour, VicinityMode vicinityMode = KEntityPixelMapper::CONNECTED_8);
	bool GetContourPoints(KGenericEntity& entity, /*OUT*/ CArray<CPoint, CPoint>& contour, VicinityMode vicinityMode = KEntityPixelMapper::CONNECTED_8);

	void GetRowPixelCount(/*OUT*/ CArray<int>& rowCounts);
	void GetColumnPixelCount(/*OUT*/ CArray<int>& columnCounts);

	~KEntityPixelMapper();

private:
	static const short unmapped = -1;

	int width, height;
	
	short** map;
	KEntityPointersArray* mappedEntities;

	inline bool IsValid(int column, int row);

	CArray<CPoint, CPoint> neighbours4;
	CArray<CPoint, CPoint> neighbours8;
	
	static void GetNeighbours(VicinityMode vicinityMode, /*OUT*/ CArray<CPoint, CPoint>& neighbours);
};

/*****************************************************************************************************************/
/*****************************************************************************************************************/

