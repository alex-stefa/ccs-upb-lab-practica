

#pragma once

#include "Entity_Utils.h"
#include "Image_Utils.h"
#include "voronoi/vzvoronoi.h"
#include "voronoi/vzdelaunay.h"
#include "../KImage/Iterators.h"
#include <map>
#include <list>


class KAreaVoronoi
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template <typename N>
	class KVoronoiCoordinate
	{
	public:
		KVoronoiCoordinate() : x(0), y(0) {}
		KVoronoiCoordinate(N x, N y) : x(x), y(y) {}
		KVoronoiCoordinate(const KVoronoiCoordinate& point) : x(point.x), y(point.y) {}

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
		KVoronoiLine() {}

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
		KVoronoiEdge(KVoronoiCell& cell1, KVoronoiCell& cell2) : cell1(&cell1), cell2(&cell2), needRecompute(true) {}

		~KVoronoiEdge() {}

		float GetMinDistance();

		inline KVoronoiCell* GetOtherCell(KVoronoiCell* cell) { return (cell == cell1) ? cell2 : cell1; }
		inline void SetNeedRecompute() { needRecompute = true; };

		typedef std::list<KVoronoiLine> LineList;

		LineList lines;
		KVoronoiCell* cell1;
		KVoronoiCell* cell2;

	private:
		bool needRecompute;
		float minDistance;
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////
	class KVoronoiCell
	{
	public:
		KVoronoiCell(KGenericEntity& entity) : entity(&entity), needRecompute(true) {}

		~KVoronoiCell() {}

		float GetMinDistance();
		KVoronoiPoint GetCenterPoint();

		inline void SetNeedRecompute()
		{ 
			needRecompute = true;
			for (EdgeMap::iterator it = edges.begin(); it != edges.end(); ++it)
				(it->second)->SetNeedRecompute();
		}

		typedef std::map<KVoronoiCell*, KVoronoiEdge*> EdgeMap;
		typedef std::list<KVoronoiSite> SiteList;

		EdgeMap edges;
		SiteList sites;
		KGenericEntity* entity;

	private:
		bool needRecompute;
		float minDistance;
	};
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
	KAreaVoronoi(KEntityPointersArray& entities, int width, int height);
	~KAreaVoronoi();

	void BuildAreaVoronoiDiagram(int sampleRate = 10, float voronoiMinDist = 2);
	void BuildDelaunayDiagram();

	int MergeEntities();

	void GetEntities(/*OUT*/ KEntityPointersArray& entities);

	void DrawVoronoiDiagram(KImage& image, KRGBColor& edgeColor = KRGBColor(0, 0, 0));

	inline int GetCellCount() { return voronoiCells.GetSize(); }
	inline KVoronoiCell& GetCell(int index) { return *(voronoiCells[index]); }

private:
	typedef std::list<KVoronoiEdge*> EdgeList;
	typedef std::map<KGenericEntity*, KVoronoiCell*> CellMap;
	typedef CArray<KVoronoiCell*, KVoronoiCell*> CellList;

	int width, height;
	CellMap cellMap;	
	CellList voronoiCells;
	EdgeList voronoiEdges;
	KEntityPointersArray* toDelete;

	KVoronoiCell* RemoveEdge(KVoronoiEdge* edge);
	bool ShouldRemoveEdge(KVoronoiEdge& edge);

	bool DeleteEdge(KVoronoiEdge* edge);
	bool IsValid(KVoronoiEdge* edge);
};
