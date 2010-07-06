/*! \addtogroup ImgEnt
 \{ */ 
//! \file Italics.h \brief Declaration file for KItalics class

#ifndef __FONT_ITALICS__
#define __FONT_ITALICS__

class KItalics
{
public:
	static double WidthMethod(KEntityPointersArray& letters, float degAngle = 16);
	static double ChainMethod(KEntityPointersArray& letters, float degAngle = 16);
};

#endif //__FONT_ITALICS__
/*! \} */