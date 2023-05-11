#pragma once
#include <map>

//================================
// Edge
//================================

class Edge {
public:
    int y_max;
    float x_min; 
    float inv_m;
    float z_min; 
    float del_z; // rate of change from z_min
    Edge *next;

public:
    Edge(int y_max, float x_min, float inv_m, float z_min, float del_z); 

    ~Edge() {}
};

//================================
// EdgeTable
//================================

class EdgeTable {
public:
    std::map<int,Edge*> scanlines;

public:
    EdgeTable() {}

    ~EdgeTable();

    int InsertEdge(int scanline, Edge* edge);

    Edge* RemoveEdge(int scanline);

    bool IsEmpty();

    void PrintEdgeTable();
};

//================================
// ActiveEdgeTable
//================================

class ActiveEdgeTable {
public:
    std::multimap<int,Edge*> *aet;

public:
    ActiveEdgeTable();

    ~ActiveEdgeTable();

    int InsertEdge(int x_int, Edge* edge);

    bool IsEmpty();

    void UpdateEdges(int delta);

    void PrintActiveEdgeTable();
};