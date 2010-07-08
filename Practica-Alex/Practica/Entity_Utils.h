

#pragma once

class KEntityUtils
{
public:
	const enum AverageMode { ARITHMETIC_MEAN = 1, HISTOGRAM_PEAK = 2 };
	static float GetAverageWidth(KEntityPointersArray& entities, AverageMode mode = KEntityUtils::HISTOGRAM_PEAK);
	static float GetAverageHeight(KEntityPointersArray& entities, AverageMode mode = KEntityUtils::HISTOGRAM_PEAK);
	static int FilterBySize(KEntityPointersArray& initialEntities, /*OUT*/ KEntityPointersArray& filteredEntities, 
		float minWidth = 0, float minHeight = 0, float maxWidth = 0, float maxHeight = 0, bool deleteFiltered = false);
	static void CopyEntityArray(KEntityPointersArray& initialEntities, /*OUT*/ KEntityPointersArray& copiedEntities);
};