/*
* The author of this software is Steven Fortune.  Copyright (c) 1994 by AT&T
* Bell Laboratories.
* Permission to use, copy, modify, and distribute this software for any
* purpose without fee is hereby granted, provided that this entire notice
* is included in all copies of any software which is or includes a copy
* or modification of this software and in all copies of the supporting
* documentation for such software.
* THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
* REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
* OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
*/

/* 
* This code was originally written by Stephan Fortune in C code.  I, Shane O'Sullivan, 
* have since modified it, encapsulating it in a C++ class and, fixing memory leaks and 
* adding accessors to the Voronoi VzEdges.
* Permission to use, copy, modify, and distribute this software for any
* purpose without fee is hereby granted, provided that this entire notice
* is included in all copies of any software which is or includes a copy
* or modification of this software and in all copies of the supporting
* documentation for such software.
* THIS SOFTWARE IS BEING PROVIDED "AS IS", WITHOUT ANY EXPRESS OR IMPLIED
* WARRANTY.  IN PARTICULAR, NEITHER THE AUTHORS NOR AT&T MAKE ANY
* REPRESENTATION OR WARRANTY OF ANY KIND CONCERNING THE MERCHANTABILITY
* OF THIS SOFTWARE OR ITS FITNESS FOR ANY PARTICULAR PURPOSE.
*/

// Dragos Tarcatu - Modified VzGraphEdge structure in order to retain the sites that generated
// an edge in the diagram.

#ifndef VORONOI_DIAGRAM_GENERATOR
#define VORONOI_DIAGRAM_GENERATOR

#include <math.h>
#include <stdlib.h>
#include <string.h>


#define DELETED -2

#define le 0
#define re 1


struct Freenode	
{
	Freenode *nextfree;
};

struct FreeNodeArrayList
{
	Freenode* memory;
	FreeNodeArrayList* next;
};

struct Freelist	
{
	Freenode *head;
	int	nodesize;
};

struct VzPointF	
{
	float x,y;
};

// structure used both for sites and for vertices 
struct VzSite	
{
	VzPointF	coord;
	int		sitenbr;
	int		refcnt;
};

struct VzEdge
{
	float   a,b,c;
	VzSite 	*ep[2];
	VzSite	*reg[2];
	int		edgenbr;
};

struct VzGraphEdge
{
	float x1,y1,x2,y2;
	VzPointF p1, p2;
	VzGraphEdge* next;
};

struct VzHalfEdge 
{
	VzHalfEdge *ELleft, *ELright;
	VzEdge *ELedge;
	int	ELrefcnt;
	char ELpm;
	VzSite *vertex;
	float ystar;
	VzHalfEdge *PQnext;
};

class VoronoiDiagramGenerator
{
public:
	VoronoiDiagramGenerator();
	~VoronoiDiagramGenerator();

	bool generateVoronoi(float *xValues, float *yValues, int numVzPointFs, float minX, float maxX, float minY, float maxY, float minDist=0);

	void resetIterator()
	{
		iteratorVzEdges = allVzEdges;
	}

	bool getNext(float& x1, float& y1, float& x2, float& y2,
		float& xs1, float& ys1, float& xs2, float& ys2)
	{
		if(iteratorVzEdges == 0)
			return false;

		x1 = iteratorVzEdges->x1;
		x2 = iteratorVzEdges->x2;
		y1 = iteratorVzEdges->y1;
		y2 = iteratorVzEdges->y2;
		xs1 = iteratorVzEdges->p1.x;
		ys1 = iteratorVzEdges->p1.y;
		xs2 = iteratorVzEdges->p2.x;
		ys2 = iteratorVzEdges->p2.y;

		iteratorVzEdges = iteratorVzEdges->next;

		return true;
	}

private:
	void cleanup();
	void cleanupVzEdges();
	char *getfree( Freelist *fl);	
	VzHalfEdge *PQfind();
	int PQempty();

	VzHalfEdge **ELhash;
	VzHalfEdge *HEcreate(), *ELleft(), *ELright(), *ELleftbnd();
	VzHalfEdge *HEcreate( VzEdge *e,int pm);

	VzPointF PQ_min();
	VzHalfEdge *PQextractmin();	
	void freeinit( Freelist *fl,int size);
	void makefree( Freenode *curr, Freelist *fl);
	void geominit();
	void plotinit();
	bool voronoi(int triangulate);
	void ref( VzSite *v);
	void deref( VzSite *v);
	void endpoint( VzEdge *e,int lr, VzSite * s);

	void ELdelete( VzHalfEdge *he);
	VzHalfEdge *ELleftbnd( VzPointF *p);
	VzHalfEdge *ELright( VzHalfEdge *he);
	void makevertex( VzSite *v);
	void out_triple( VzSite *s1,  VzSite *s2, VzSite * s3);

	void PQinsert( VzHalfEdge *he, VzSite * v, float offset);
	void PQdelete( VzHalfEdge *he);
	bool ELinitialize();
	void ELinsert(	VzHalfEdge *lb,  VzHalfEdge *newHe);
	VzHalfEdge * ELgethash(int b);
	VzHalfEdge *ELleft( VzHalfEdge *he);
	VzSite *leftreg( VzHalfEdge *he);
	void out_site( VzSite *s);
	bool PQinitialize();
	int PQbucket( VzHalfEdge *he);
	void clip_line( VzEdge *e);
	char *myalloc(unsigned n);
	int right_of( VzHalfEdge *el, VzPointF *p);

	VzSite *rightreg( VzHalfEdge *he);
	VzEdge *bisect(	VzSite *s1,	VzSite *s2);
	float dist( VzSite *s, VzSite *t);
	VzSite *intersect( VzHalfEdge *el1,  VzHalfEdge *el2,  VzPointF *p=0);

	void out_bisector( VzEdge *e);
	void out_ep( VzEdge *e);
	void out_vertex( VzSite *v);
	VzSite *nextone();

	void pushVzGraphEdge(float x1, float y1, float x2, float y2,  VzSite* s1,  VzSite* s2);

	void openpl();
	void line(float x1, float y1, float x2, float y2,  VzSite* s1,  VzSite* s2);
	void circle(float x, float y, float radius);
	void range(float minX, float minY, float maxX, float maxY);

	Freelist	hfl;
	VzHalfEdge *ELleftend, *ELrightend;
	int 	ELhashsize;

	int		triangulate, sorted, plot, debug;
	float	xmin, xmax, ymin, ymax, deltax, deltay;

	VzSite	*sites;
	int		nsites;
	int		siteidx;
	int		sqrt_nsites;
	int		nvertices;
	Freelist sfl;
	VzSite	*bottomsite;

	int		nedges;
	Freelist efl;
	int		PQhashsize;
	VzHalfEdge *PQhash;
	int		PQcount;
	int		PQmin;

	int		ntry, totalsearch;
	float	pxmin, pxmax, pymin, pymax, cradius;
	int		total_alloc;

	float borderMinX, borderMaxX, borderMinY, borderMaxY;

	FreeNodeArrayList* allMemoryList;
	FreeNodeArrayList* currentMemoryBlock;

	VzGraphEdge* allVzEdges;
	VzGraphEdge* iteratorVzEdges;

	float minDistanceBetweenVzSites;
};

int scomp(const void *p1,const void *p2);


#endif


