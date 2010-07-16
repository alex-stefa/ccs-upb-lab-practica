

#pragma once

#include "Entity_Utils.h"
#include "voronoi/vzvoronoi.h"
#include "voronoi/vzdelaunay.h"


class KVoronoiCell;

class KVoronoiPoint
{
public:
	KVoronoiPoint() : x(0), y(0) {}
	KVoronoiPoint(float x, float y) : x(x), y(y) {}
	KVoronoiPoint(KVoronoiPoint& point) : x(point.x), y(point.y) {}

	~KVoronoiPoint() {}

	float x, y;
};

class KVoronoiEdge
{
public:
	KVoronoiEdge(KVoronoiCell* cell1, KVoronoiCell* cell2, KVoronoiPoint& pt1, KVoronoiPoint& pt2) 
		: cell1(cell1), cell2(cell2), pt1(pt1), pt2(pt2) 
		{ ASSERT(cell1 != NULL); ASSERT(cell2 != NULL); }

	~KVoronoiEdge() {}

	KVoronoiPoint pt1;
	KVoronoiPoint pt2;
	KVoronoiCell* cell1;
	KVoronoiCell* cell2;
};

class KVoronoiCell
{
public:
	KVoronoiCell(KGenericEntity* entity) 
		: entity(entity)
		{ ASSERT(entity != NULL); edges = new CArray<KVoronoiEdge*, KVoronoiEdge*>(); }
	
	~KVoronoiCell() { delete edges; }
	
	KGenericEntity* entity;
	CArray<KVoronoiEdge*, KVoronoiEdge*>* edges;

	KVoronoiPoint GetCenterPoint();
};

class KAreaVoronoi
{
public:
	KAreaVoronoi(KImage& image);
	~KAreaVoronoi();

	void BuildAreaVoronoiDiagram(int sampleRate = 10, float voronoiMinDist = 2);
	void BuildDelaunayDiagram();

	void GetVoronoiCells(CArray<KVoronoiCell*, KVoronoiCell*>& cells);
	void GetVoronoiEdges(CArray<KVoronoiEdge*, KVoronoiEdge*>& edges);

private:
	int width, height;
	KImagePage* pImgPage;
	KHash2D* hash;
	std::map<KGenericEntity*, KVoronoiCell*> cellMap;	
	CArray<KVoronoiCell*, KVoronoiCell*>* voronoiCells;
	CArray<KVoronoiEdge*, KVoronoiEdge*>* voronoiEdges;
};
