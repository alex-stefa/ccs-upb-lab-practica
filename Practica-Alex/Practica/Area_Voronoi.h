

#pragma once

#include "Entity_Utils.h"
#include "voronoi/vzvoronoi.h"
#include "voronoi/vzdelaunay.h"
#include <map>

class KAreaVoronoi
{
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename N>
	class KVoronoiCoordinate
	{
	public:
		KVoronoiCoordinate() : x(0), y(0) {}
		KVoronoiCoordinate(N x, N y) : x(x), y(y) {}
		KVoronoiCoordinate(KVoronoiCoordinate& point) : x(point.x), y(point.y) {}

		~KVoronoiCoordinate() {}

		N x, y;
	};

	typedef KVoronoiCoordinate<float> KVoronoiPoint;
	typedef KVoronoiCoordinate<short> KVoronoiSite;
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class KVoronoiLine
	{
	public:
		KVoronoiLine(float xp1, float yp1, float xp2, float yp2, short xs1, short ys1, short xs2, short ys2) 
			: point1(xp1, yp1), point2(xp2, yp2), site1(xs1, ys1), site2(xs2, ys2) {}
		KVoronoiLine(KVoronoiPoint& point1, KVoronoiPoint& point2, KVoronoiSite& site1, KVoronoiSite& site2) 
			: point1(point1), point2(point2), site1(site1), site2(site2) {}

		~KVoronoiLine() {}

		KVoronoiSite site1;
		KVoronoiSite site2;
		KVoronoiPoint point1;
		KVoronoiPoint point2;
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class KVoronoiCell;

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
				lines = NULL;
			}
		}

		float GetMinDistance();

		inline KVoronoiCell* GetOtherCell(KVoronoiCell* cell) { return (cell == cell1) ? cell2 : cell1; }

		KVoronoiCell* cell1;
		KVoronoiCell* cell2;
		CArray<KVoronoiLine*, KVoronoiLine*>* lines;
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class KVoronoiCell
	{
	public:
		typedef std::map<KVoronoiCell*, KVoronoiEdge*> EdgeMap;

		KVoronoiCell(KGenericEntity& entity) : entity(&entity) {}

		~KVoronoiCell()
		{
			for (EdgeMap::iterator it = edges.begin(); it != edges.end(); ++it)
				if (it->second->cell1 == this)
					delete it->second;
		}

		float GetMinDistance();
		KVoronoiPoint GetCenterPoint();

		KGenericEntity* entity;
		EdgeMap edges;
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////


public:
	KAreaVoronoi(KEntityPointersArray& entities, int width, int height);
	~KAreaVoronoi();

	void BuildAreaVoronoiDiagram(int sampleRate = 10, float voronoiMinDist = 2);
	void BuildDelaunayDiagram();

	KVoronoiCell* MergeCells(int index1, int index2);

	inline int GetCellCount() { return voronoiCells->GetSize(); }
	inline KVoronoiCell& GetCell(int index) { return *(voronoiCells->GetAt(index)); }

private:
	int width, height;
	std::map<KGenericEntity*, KVoronoiCell*> cellMap;	
	CArray<KVoronoiCell*, KVoronoiCell*>* voronoiCells;
	KEntityPointersArray* toDelete;
};
