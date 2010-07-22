/*! \addtogroup Imgent
 \{ */ 
//! \file Text_Lines.h \brief Declaration file for KTextFilters class

#ifndef __TEXT_LINES__
#define __TEXT_LINES__

class KTextLines
{
public:
	static void BuildLines(KEntityPointersArray& letters, /*OUT*/ KEntityPointersArray& lines);
	static void DoCleanup();

	/* DEBUG */
	static bool DebugEnabled;
	static char* DebugOutputPath;
	static char* DebugFilenamePrefix;

protected:
	static KEntityPointersArray* toDelete;
};

#endif  //__TEXT_LINES_
/*  \} */