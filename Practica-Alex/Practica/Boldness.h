/*! \addtogroup ImgEnt
 \{ */ 
//! \file Boldness.h \brief Declaration file for KBoldness class

#ifndef __FONT_BOLDNESS__
#define __FONT_BOLDNESS__

class KBoldness 
{
public:
	static double BlackPercentage(KEntityPointersArray& letters);
	static double ContourLength(KEntityPointersArray& letters);
	static double MaxPenWidth(KEntityPointersArray& letters);
	static double AvgPenWidth(KEntityPointersArray& letters);
	static double ShortestSegment(KEntityPointersArray& letters);
	static double DomCrosshairSize(KEntityPointersArray& letters);
	static double AvgCrosshairSize(KEntityPointersArray& letters);

	/* DEBUG */
	static bool DebugEnabled;
	static char* DebugOutputPath;
	static char* CrosshairHistogramFilename;
};

#endif //__FONT_BOLDNESS__
/*! \} */
