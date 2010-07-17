

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

class KVoronoiLine
{
public:
	KVoronoiLine(float x1, float y1, float x2, float y2) : pt1(x1, y1), pt2(x2, y2) {}
	KVoronoiLine(KVoronoiPoint& pt1, KVoronoiPoint& pt2) : pt1(pt1), pt2(pt2) {}

	~KVoronoiLine() {}

	KVoronoiPoint pt1;
	KVoronoiPoint pt2;
};

class KVoronoiEdge
{
public:
	KVoronoiEdge(KVoronoiCell& cell1, KVoronoiCell& cell2) : cell1(&cell1), cell2(&cell2)
	{
		lines = new CArray<KVoronoiLine*, KVoronoiLine*>();
	}

	~KVoronoiEdge()
	{ 
		if (lines != NULL)
		{
			for (int i = 0; i < lines->GetSize(); ++i)
				delete lines->GetAt(i);
			delete lines;
			lines == NULL;
		}
	}

	KVoronoiCell* cell1;
	KVoronoiCell* cell2;

	CArray<KVoronoiLine*, KVoronoiLine*>* lines;
};

class KVoronoiCell
{
public:
	KVoronoiCell(KGenericEntity& entity) : entity(&entity) {}
	
	~KVoronoiCell()
	{
		for (std::map<KVoronoiCell*, KVoronoiEdge*>::iterator it = edges.begin(); it != edges.end(); ++it)
			if (it->second->cell1 == this)
				delete it->second;
	}
	
	KGenericEntity* entity;
	
	std::map<KVoronoiCell*, KVoronoiEdge*> edges;

	KVoronoiPoint GetCenterPoint();
};

class KAreaVoronoi
{
public:
	KAreaVoronoi(KEntityPointersArray& entities, int width, int height);
	~KAreaVoronoi();

	void GetEntities(/*OUT*/ KEntityPointersArray& entities);
	int GetEntityCount();

	void BuildAreaVoronoiDiagram(int sampleRate = 10, float voronoiMinDist = 2);
	void BuildDelaunayDiagram();

	void GetVoronoiCells(/*OUT*/ CArray<KVoronoiCell*, KVoronoiCell*>& cells);

private:
	int width, height;
	std::map<KGenericEntity*, KVoronoiCell*> cellMap;	
	CArray<KVoronoiCell*, KVoronoiCell*>* voronoiCells;
};
