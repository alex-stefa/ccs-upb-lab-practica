/*! \addtogroup Imgent
 \{ */ 
//! \file Text_Filters.h \brief Declaration file for KTextFilters class

#ifndef __TEXT_FILTERS__
#define __TEXT_FILTERS__

class KTextFilters 
{
public:
	static KEntityPointersArray FilterLetters(KEntityPointersArray& letters, 
		bool useWidthFilter = true, bool useInsideFilter = true, bool useMergeFilter = true);

	/* DEBUG */
	static bool DebugEnabled;
	static char* DebugOutputPath;
	static char* BoundingBoxDiagramFilename;
};

#endif  //__TEXT_FILTERS__
/*  \} */