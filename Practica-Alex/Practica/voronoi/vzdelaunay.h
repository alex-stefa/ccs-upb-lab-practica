/********************************************************************************
	Copyright (C) 2004-2005 Sjaak Priester	

	This is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Tinter; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************************************/

// Delaunay
// Class to perform Delaunay triangulation on a set of vertices
//
// Version 1.1 (C) 2005, Sjaak Priester, Amsterdam.
// - Removed bug which gave incorrect results for co-circular vertices.
//
// Version 1.0 (C) 2004, Sjaak Priester, Amsterdam.
// mailto:sjaak@sjaakpriester.nl


#ifndef VZDELAUNAY_H
#define VZDELAUNAY_H

#include <set>

#define qreal float


class VzPointF
{
public:
	VzPointF() : _x(0), _y(0) {}
	VzPointF(qreal x, qreal y) : _x(x), _y(y) {}
	VzPointF(const VzPointF& point) : _x(point._x), _y(point._y) {}
	~VzPointF() {}

	qreal x() const { return _x; }
	qreal y() const { return _y; }
	
	void setX(qreal x) { _x = x; }
	void setY(qreal y) { _y = y; }

	bool operator==(VzPointF& point)  { return _x == point._x && _y == point._y; }
	bool operator!=(VzPointF& point)  { return _x != point._x || _y != point._y;}
	
	void operator+=(VzPointF point) { _x += point._x; _y += point._y; }
	void operator-=(VzPointF point) { _x -= point._x; _y -= point._y; }

	VzPointF operator+(const VzPointF point) const { return VzPointF(_x + point._x, _y + point._y); }
	VzPointF operator-(const VzPointF point) const { return VzPointF(_x - point._x, _y - point._y); }
	VzPointF operator-()  { return VzPointF(-_x, -_y); }

private:
	qreal _x, _y;
};


class VzVertex
{
public:
    VzVertex()							: point(0, 0)                   {}
    VzVertex(const VzVertex& vertex)    : point(vertex.point)           {}
    VzVertex(const VzPointF& point)     : point(point)                  {}
    VzVertex(qreal x, qreal y)          : point(x, y)                   {}

    bool operator < (const VzVertex& v) const
    {
        if (point.x() == v.point.x())
            return point.y() < v.point.y();
        return point.x() < v.point.x();
    }

    bool operator == (const VzVertex& v) const
    {
        return point.x() == v.point.x() && point.y() == v.point.y();
    }

    qreal GetX() const        { return point.x(); }
    qreal GetY() const        { return point.y(); }

    void SetX(int x)        { point.setX((qreal) x); }
    void SetY(int y)        { point.setY((qreal) y); }

    const VzPointF& GetPoint() const { return point; }

    void dump() const;

private:
    VzPointF point;
};


typedef std::set<VzVertex> VertexSet;


class VzTriangle
{
public:
    VzTriangle(const VzVertex* p0, const VzVertex* p1, const VzVertex* p2)
    {
        vertices[0] = p0;
        vertices[1] = p1;
        vertices[2] = p2;
        SetCircumCircle();
    }

    VzTriangle(const VzVertex* pV)
    {
        for (int i = 0; i < 3; i++)
            vertices[i] = pV++;
        SetCircumCircle();
    }

    bool operator < (const VzTriangle& tri) const
    {
        if (center.x() == tri.center.x())
            return center.y() < tri.center.y();
        return center.x() < tri.center.x();
    }

    const VzVertex* GetVertex(int i) const
    {
        ASSERT(i >= 0 && i < 3);
        return vertices[i];
    }

    bool IsLeftOf(VertexSet::const_iterator vIt) const
    {
        // returns true if * itVertex is to the right of the triangle's circumcircle
        return vIt->GetPoint().x() > (center.x() + radius);
    }

    // Returns true if * itVertex is in the triangle's circumcircle.
    // A vertex exactly on the circle is also considered to be in the circle.
    bool CCEncompasses(VertexSet::const_iterator vIt) const
    {
        VzPointF dist = vIt->GetPoint() - center;
        qreal dist2 = dist.x() * dist.x() + dist.y() * dist.y();
        return dist2 <= radius * radius;
    }

    VzPointF GetCenter() const
    {
        return center;
    }

    qreal GetRadius() const
    {
        return radius;
    }

    void dump() const;

private:
    const VzVertex* vertices[3];
    VzPointF center;
    qreal radius;
    qreal radius2;

    void SetCircumCircle();
};



typedef std::multiset<VzTriangle> TriangleSet;


class VzEdge
{
public:
    VzEdge(const VzVertex* v1, const VzVertex* v2) :
        v1(v1), v2(v2)
    {
    }

    bool operator < (const VzEdge& e) const
    {
        if (v1 == e.v1)
            return *v2 < *e.v2;
        return *v1 < *e.v1;
    }

    void dump() const;

public:
    const VzVertex* v1;
    const VzVertex* v2;
};

typedef std::set<VzEdge> EdgeSet;


class Delaunay
{
public:
    // Calculate the Delaunay triangulation for the given set of vertices.
    void Triangulate(const VertexSet& vertices, TriangleSet& output);

    // Put the edges of the triangles in an edgeSet, eliminating double edges.
	// This comes in useful for drawing the triangulation.
    void TrianglesToEdges(const TriangleSet& triangles, EdgeSet& edges);
private:
    void HandleEdge(const VzVertex* v1, const VzVertex* v2, EdgeSet& edges);
};


#endif

