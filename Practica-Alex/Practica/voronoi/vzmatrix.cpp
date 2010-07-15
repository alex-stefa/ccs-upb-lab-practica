
#include "StdAfx.h"
#include "vzmatrix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


VzMatrix::VzMatrix(const CSize& size) :
		nrows(size.cx),
        ncols(size.cy),
        data(nrows * ncols, 0) { }

VzMatrix::VzMatrix(short nrows, short ncols) :
        nrows(nrows),
        ncols(ncols),
        data(nrows * ncols, 0) { }

VzMatrix::~VzMatrix() { }

short& VzMatrix::at(short row, short col)
{
    ASSERT(row >= 0 && row < nrows);
    ASSERT(col >= 0 && col < ncols);

    return data[ncols*row + col];
}

const short& VzMatrix::at(short row, short col) const
{
    ASSERT(row >= 0 && row < nrows);
    ASSERT(col >= 0 && col < ncols);

    return data[ncols*row + col];
}

short& VzMatrix::at(const POINTS& point)
{
    ASSERT(point.x >= 0 && point.x < nrows);
    ASSERT(point.y >= 0 && point.y < ncols);

    return data[ncols * point.x + point.y];
}

const short& VzMatrix::at(const POINTS& point) const
{
    ASSERT(point.x >= 0 && point.x < nrows);
    ASSERT(point.y >= 0 && point.y < ncols);

    return data[ncols * point.x + point.y];
}


CSize VzMatrix::size() const
{
    return CSize(nrows, ncols);
}


short VzMatrix::rows() const
{
    return nrows;
}


short VzMatrix::cols() const
{
    return ncols;
}


bool VzMatrix::valid(const POINTS& point) const
{
    return (point.x >= 0 && point.x < nrows &&
            point.y >= 0 && point.y < ncols);
}
