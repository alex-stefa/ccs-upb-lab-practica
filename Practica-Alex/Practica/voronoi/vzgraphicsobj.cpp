
#include "StdAfx.h"
#include "vzgraphicsobj.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;


void VzGraphicsObj::add(const POINTS& point)
{
	points.push_back(point);
}


int VzGraphicsObj::size() const
{
	return points.size();
}


VzGraphicsObj::iterator VzGraphicsObj::begin()
{
    return points.begin();
}


VzGraphicsObj::const_iterator VzGraphicsObj::begin() const
{
    return points.begin();
}


VzGraphicsObj::iterator VzGraphicsObj::end()
{
    return points.end();
}


VzGraphicsObj::const_iterator VzGraphicsObj::end() const
{
    return points.end();
}
