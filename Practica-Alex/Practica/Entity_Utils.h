

#pragma once

class KEntityUtils
{
public:
	const enum AverageMode { ARITHMETIC_MEAN = 1, HISTOGRAM_PEAK = 2 };
	static float GetAverageWidth(KEntityPointersArray& entities, AverageMode mode = KEntityUtils::HISTOGRAM_PEAK);
	static float GetAverageHeight(KEntityPointersArray& entities, AverageMode mode = KEntityUtils::HISTOGRAM_PEAK);
	
	static int FilterBySize(KEntityPointersArray& initialEntities, 
		/*OUT*/ KEntityPointersArray& filteredEntities, 
		float minWidth = 0, float minHeight = 0, float maxWidth = 0, float maxHeight = 0, bool deleteFiltered = false);
	static int FilterBySize(KEntityPointersArray& initialEntities, 
		/*OUT*/ KEntityPointersArray& filteredEntities, KEntityPointersArray& removedEntities,
		float minWidth = 0, float minHeight = 0, float maxWidth = 0, float maxHeight = 0);
	
	static void CopyEntityArray(KEntityPointersArray& initialEntities, 
		/*OUT*/ KEntityPointersArray& copiedEntities);
	
	static void GenerateEntityContours(KGenericEntity& entity, 
		/*OUT*/ KPointSet& exteriorPoints, KPointSet& interiorPoints);
};


class KEntityPixelMapper
{
public:
	KEntityPixelMapper(int width, int height);

	void AddEntity(KGenericEntity& entity, bool assertNoOverlapping = true);
	void AddEntityArray(KEntityPointersArray& entities, bool assertNoOverlapping = true);

	bool RemoveEntity(KGenericEntity& entity);
	int RemoveEntityArray(KEntityPointersArray& entities);

	KGenericEntity* GetEntityAtPixel(int column, int row);
	KGenericEntity* GetEntityAtPixel(CPoint& point);

	void GetEntities(/*OUT*/ KEntityPointersArray& entities);
	void GetEntities(CRect& rect, /*OUT*/ KEntityPointersArray& entities);
	int GetEntityCount();
	int GetEntityCount(CRect& rect);

	const enum VicinityMode { CONNECTED_4 = 1, CONNECTED_8 = 2 };
	void GetContourPoints(/*OUT*/ CArray<CPoint, CPoint>& contour, VicinityMode vicinityMode = KEntityPixelMapper::CONNECTED_8);
	bool GetContourPoints(KGenericEntity& entity, /*OUT*/ CArray<CPoint, CPoint>& contour, VicinityMode vicinityMode = KEntityPixelMapper::CONNECTED_8);
		
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


