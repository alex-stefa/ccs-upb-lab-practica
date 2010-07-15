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
	along with this application; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
********************************************************************************/

// Delaunay
// Class to perform Delaunay triangulation on a set of vertices
//
// Version 1.2 (C) 2005, Sjaak Priester, Amsterdam.
// - Removed stupid bug in SetY; function wasn't used, so no consequences. Thanks to squat.
//
// Version 1.1 (C) 2005, Sjaak Priester, Amsterdam.
// - Removed bug which gave incorrect results for co-circular vertices.
//
// Version 1.0 (C) 2004, Sjaak Priester, Amsterdam.
// mailto:sjaak@sjaakpriester.nl


#include "StdAfx.h"
#include "vzdelaunay.h"

#include <cmath>
#include <iostream>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;


const qreal EPS = 1.192092896e-07;

const qreal sqrt3 = 1.732050808F;


void VzVertex::dump() const
{
    cout << "VzVertex: (" << point.x() << "," << point.y() << ")" << endl;
}


void VzTriangle::SetCircumCircle()
{
    qreal x0 = vertices[0]->GetX();
	qreal y0 = vertices[0]->GetY();

	qreal x1 = vertices[1]->GetX();
	qreal y1 = vertices[1]->GetY();

	qreal x2 = vertices[2]->GetX();
	qreal y2 = vertices[2]->GetY();

    qreal y10 = y1 - y0;
	qreal y21 = y2 - y1;

    bool b10zero = y10 > -EPS && y10 < EPS;
    bool b21zero = y21 > -EPS && y21 < EPS;

    if (b10zero)
    {
        if (b21zero)
        {
            if (x1 > x0)
            {
                if (x2 > x1) x1 = x2;
            }
            else
            {
                if (x2 < x0) x0 = x2;
            }
            center.setX((x0 + x1) * 0.5);
            center.setY(y0);
        }
        else
        {
            qreal m1 =  (x1 - x2) / y21;
            qreal mx1 = (x1 + x2) * 0.5;
            qreal my1 = (y1 + y2) * 0.5;
            center.setX((x0 + x1) * 0.5);
            center.setY(m1 * (center.x() - mx1) + my1);

        }
    }
    else if (b21zero)
    {
        qreal m0 = (x0 - x1) / y10;
        qreal mx0 = (x0 + x1) * 0.5;
        qreal my0 = (y0 + y1) * 0.5;
        center.setX((x1 + x2) * 0.5);
        center.setY(m0 * (center.x() - mx0) + my0);
    }
    else
    {
        qreal m0 = (x0 - x1) / y10;
        qreal m1 = (x1 - x2) / y21;
        qreal mx0 = (x0 + x1) * 0.5;
        qreal my0 = (y0 + y1) * 0.5;
        qreal mx1 = (x1 + x2) * 0.5;
        qreal my1 = (y1 + y2) * 0.5;
        center.setX((m0 * mx0 - m1 * mx1 + my1 - my0) / (m0 - m1));
        center.setY(m0 * (center.x() - mx0) + my0);
    }

    qreal dx = x0 - center.x();
    qreal dy = y0 - center.y();

    radius2 = dx * dx + dy * dy;
    radius = sqrt(radius2);
    //make radius2 slightly higher to ensure that all edges
	// of co-circular vertices will be caught.
	// Note that this is a compromise. In fact, the algorithm isn't really
	// suited for very many co-circular vertices.
    radius2 *= 1.000001f;
}


void VzTriangle::dump() const
{
    cout << "######VzTriangle#####:" << endl;
    vertices[0]->dump();
    vertices[1]->dump();
    vertices[2]->dump();
    cout << "######################" << endl;
}


// Function object to check whether a triangle has one of the vertices in SuperTriangle.
// operator() returns true if it does.

class TriangleHasVertex
{
public:
    TriangleHasVertex(const VzVertex superTriangle[3]) :
        superTriangle(superTriangle)
    {
    }

    bool operator() (const VzTriangle& tri) const
    {
        for (int i = 0; i < 3; i++)
        {
            const VzVertex* v = tri.GetVertex(i);
            if (v >= superTriangle && v < (superTriangle + 3))
                return true;
        }
        return false;
    }
protected:
    const VzVertex* superTriangle;
};



// Function object to check whether a triangle is 'completed', i.e. doesn't need to be checked
// again in the algorithm, i.e. it won't be changed anymore.
// Therefore it can be removed from the workset.
// A triangle is completed if the circumcircle is completely to the left of the current vertex.
// If a triangle is completed, it will be inserted in the output set, unless one or more of it's vertices
// belong to the 'super triangle'.

class TriangleIsCompleted
{
public:
    TriangleIsCompleted(VertexSet::const_iterator vIt,
                        TriangleSet& output,
                        const VzVertex superTriangle[3])
        : vIt(vIt)
        , output(output)
        , superTriangle(superTriangle)
    {
    }

    bool operator () (const VzTriangle& tri) const
    {
        bool b = tri.IsLeftOf(vIt);
        if (b)
        {
            TriangleHasVertex thv(superTriangle);
            if (!thv(tri))
                output.insert(tri);
        }
        return b;
    }

private:
    VertexSet::const_iterator vIt;
    TriangleSet& output;
    const VzVertex* superTriangle;
};


// Function object to check whether vertex is in circumcircle of triangle.
// operator() returns true if it does.
// The edges of a 'hot' triangle are stored in the edgeSet edges.

class VertexIsInCircumCircle
{
public:
    VertexIsInCircumCircle(VertexSet::const_iterator vIt, EdgeSet& edges)
        : vIt(vIt)
        , edges(edges)
    {
    }

    bool operator () (const VzTriangle& tri) const
    {
        bool b = tri.CCEncompasses(vIt);
        if (b)
        {
            HandleEdge(tri.GetVertex(0), tri.GetVertex(1));
            HandleEdge(tri.GetVertex(1), tri.GetVertex(2));
            HandleEdge(tri.GetVertex(2), tri.GetVertex(0));
        }
        return b;
    }

private:
    void HandleEdge(const VzVertex* v1, const VzVertex* v2) const
    {
        const VzVertex* pV1(0);
        const VzVertex* pV2(0);

        // Create a normalized edge, in which the smallest vertex comes first.

        if (*v1 < *v2)
        {
            pV1 = v1;
            pV2 = v2;
        }
        else
        {
            pV1 = v2;
            pV2 = v1;
        }

        VzEdge e(pV1, pV2);

        // Check if this edge is already in the buffer
        EdgeSet::iterator found = edges.find(e);
        if (found == edges.end())
            edges.insert(e);
        else
            edges.erase(found);
    }

    VertexSet::const_iterator vIt;
    EdgeSet& edges;
};


void VzEdge::dump() const
{
    cout << "########VzEdge#######:" << endl;
    v1->dump();
    v2->dump();
    cout << "######################" << endl;

}


void Delaunay::Triangulate(const VertexSet& vertices, TriangleSet& output)
{
    if (vertices.size() < 3)
        return;

    // Determine the bounding box.
    VertexSet::const_iterator vIt = vertices.begin();
    qreal xMin = vIt->GetX();
    qreal yMin = vIt->GetY();
    qreal xMax = xMin;
    qreal yMax = yMin;

    vIt++;

    for (; vIt != vertices.end(); vIt++)
    {
        xMax = vIt->GetX();
        qreal y = vIt->GetY();
        if (y < yMin) yMin = y;
        if (y > yMax) yMax = y;
    }

    qreal dx = xMax - xMin;
    qreal dy = yMax - yMin;


    qreal ddx = dx * 0.01;
    qreal ddy = dy * 0.01;

    xMin -= ddx;
    xMax += ddx;
    dx += 2 * ddx;

    yMin -= ddy;
    yMax += ddy;
    dy += 2 * ddy;

    // Create a 'super triangle', encompassing all the vertices. We choose an equilateral triangle with horizontal base.
	// We could have made the 'super triangle' simply very big. However, the algorithm is quite sensitive to
	// rounding errors, so it's better to make the 'super triangle' just big enough, like we do here.

    VzVertex vSuper[3];
    vSuper[0] = VzVertex(xMin - dy * sqrt3 / 3.0, yMin);
    vSuper[1] = VzVertex(xMax + dy * sqrt3 / 3.0, yMin);
    vSuper[2] = VzVertex((xMin + xMax) * 0.5, yMax + dx * sqrt3 * 0.5);

    TriangleSet workset;
    workset.insert(VzTriangle(vSuper));

    for (vIt = vertices.begin(); vIt != vertices.end(); vIt++)
    {
        // First, remove all 'completed' triangles from the workset.
		// A triangle is 'completed' if its circumcircle is entirely to the left of the current vertex.
		// Vertices are sorted in x-direction (the set container does this automagically).
		// Unless they are part of the 'super triangle', copy the 'completed' triangles to the output.
		// The algorithm also works without this step, but it is an important optimalization for bigger numbers of vertices.
		// It makes the algorithm about five times faster for 2000 vertices, and for 10000 vertices,
		// it's thirty times faster. For smaller numbers, the difference is negligible.

        TriangleIsCompleted completed(vIt, output, vSuper);
        TriangleSet::iterator wIt = workset.begin();
        while (wIt != workset.end()) {
            TriangleSet::iterator next = wIt; next++;
            if (completed(*wIt)){
                workset.erase(wIt);
            }
            wIt = next;
        }

        // A triangle is 'hot' if the current vertex v is inside the circumcircle.
		// Remove all hot triangles, but keep their edges.

        EdgeSet edges;
        VertexIsInCircumCircle contained(vIt, edges);
        wIt = workset.begin();
        while (wIt != workset.end()) {
            TriangleSet::iterator next = wIt; next++;
            if (contained(*wIt)) {
                workset.erase(wIt);
            }
            wIt = next;
        }

        // Create new triangles from the edges and the current vertex.
        for (EdgeSet::iterator eIt = edges.begin(); eIt != edges.end(); eIt++) {
            VzTriangle tri(eIt->v1, eIt->v2, &(*vIt));
            workset.insert(tri);
        }
    }

    // Finally, remove all the triangles belonging to the 'super triangle' and move the remaining
	// triangles tot the output; remove_copy_if lets us do that in one go.
    TriangleSet::iterator where = output.begin();
    remove_copy_if(workset.begin(), workset.end(), inserter(output, where),
                   TriangleHasVertex(vSuper));

}


void Delaunay::TrianglesToEdges(const TriangleSet& triangles, EdgeSet& edges)
{
    for (TriangleSet::const_iterator tIt = triangles.begin();
                tIt != triangles.end(); tIt++)
    {
        HandleEdge(tIt->GetVertex(0), tIt->GetVertex(1), edges);
        HandleEdge(tIt->GetVertex(1), tIt->GetVertex(2), edges);
        HandleEdge(tIt->GetVertex(2), tIt->GetVertex(0), edges);
    }
}


void Delaunay::HandleEdge(const VzVertex* v1, const VzVertex* v2, EdgeSet& edges)
{
    const VzVertex* pV1 = 0;
    const VzVertex* pV2 = 0;

    if (*v1 < *v2)
    {
        pV1 = v1;
        pV2 = v2;
    }
    else
    {
        pV1 = v2;
        pV2 = v1;
    }
    // Insert a normalized edge. If it's already in edges, insertion will fail,
	// thus leaving only unique edges.
    edges.insert(VzEdge(pV1, pV2));

}

