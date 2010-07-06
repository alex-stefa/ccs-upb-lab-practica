/*! \addtogroup Imgent
\{ */ 
//! \file Text_Filters.cpp \brief Implementation file for KTextFilters class

#include "StdAfx.h"
#include "Text_Filters.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LEVEL0_CHECK(letters) \
	if (letters.GetSize() == 0) return KEntityPointersArray(); \
	CString* baseType = letters[0]->GetDirectBaseType(); \
	if (baseType && *baseType != CCS_LEVEL0) \
	{ TRACE("Input entities are not LEVEL0 type!\n");  ASSERT(false); return KEntityPointersArray(); } 


bool KTextFilters::DebugEnabled = false;
char* KTextFilters::DebugOutputPath = ".\\";
char* KTextFilters::BoundingBoxDiagramFilename = "boundingbox";


//! Returns number of pixels inside a level0 entity (seems KEntity.intNumberOfPixels is never assigned!)
static int NumberOfPixels(KEntity& entity)
{
	KRowSegment* segment;
	int pixelCount = 0;

	for (int i = entity.GetNumberOfSegments()-1; i >= 0; --i)
	{
		segment = entity.Segment(i);
		pixelCount += abs(segment->intStopColumn - segment->intStartColumn) + 1;
	}

	return pixelCount;
}


KEntityPointersArray KTextFilters::FilterLetters(KEntityPointersArray& letters, 
		bool useWidthFilter, bool useInsideFilter, bool useMergeFilter)
{
	LEVEL0_CHECK(letters);



	return KEntityPointersArray();
}

/*  \} */