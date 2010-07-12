/*! \addtogroup Imgent
 \{ */ 
//! \file Text_Filters.h \brief Declaration file for KTextFilters class

#ifndef __TEXT_FILTERS__
#define __TEXT_FILTERS__

class KTextFilters 
{
public:
	static void FilterLetters(KEntityPointersArray& letters, 
		bool useWidthFilter = true, bool useInsideFilter = true, bool useMergeFilter = true);
	static void DoCleanup();

	/* DEBUG */
	static bool DebugEnabled;
	static char* DebugOutputPath;
	static char* DebugFilenamePrefix;

protected:
	static KEntityPointersArray* toDelete;
	static void InsideFilter(KEntityPointersArray& initialEntities, /*OUT*/ KEntityPointersArray& filteredEntities);
};

#endif  //__TEXT_FILTERS__
/*  \} */