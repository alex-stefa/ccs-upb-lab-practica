/*! \addtogroup ImgEnt
 \{ */ 
//! \file Font_Size.h \brief Declaration file for KFontSize class

#ifndef __TEXT_FONT_SIZE__
#define __TEXT_FONT_SIZE__

#include "Discrete_Histogram.h"

class KFontSize
{
public:
	static KHistogram GetFontSizeHistogram(KEntityPointersArray& letters);
	static void GetFontSize(KEntityPointersArray& letters, /*OUT*/ int* lowCaps, int* highCaps);

	/* DEBUG */
	static bool DebugEnabled;
	static char* DebugOutputPath;
	static char* FontSizeHistogramFilename;
};

#endif //__TEXT_FONT_SIZE__
/*! \} */