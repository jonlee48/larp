#include "edgetable.h"
#include <stdio.h>
#include <assert.h>
#include <cmath>

//=============================================
// Edge 
//=============================================
Edge::Edge(int y_max, double x_min, double inv_m, double z_min, double del_z) {
    this->y_max = y_max;
    this->x_min = x_min;
    this->inv_m= inv_m;
    this->z_min = z_min;
    this->del_z = del_z;
    this->next = nullptr;
}

//=============================================
// Edge Table
//=============================================

int EdgeTable::InsertEdge(int scanline, Edge* edge) {
    std::map<int,Edge*>::iterator head;

    head = this->scanlines.find(scanline);
    if (head == this->scanlines.end()) {
        // Scanline is empty, insert edge to head
        this->scanlines[scanline] = edge;
    }
    else {
        // Scanline contains edges, add edge in sorted order
        Edge* cur = head->second;

        if (edge->x_min < cur->x_min || (edge->x_min == cur->x_min && edge->inv_m < cur->inv_m)) {
            // Insert edge at head (if smaller than head)
            this->scanlines[scanline] = edge;
            this->scanlines[scanline]->next = cur;
        }
        else {
            // Insert edge in sorted order
            while(cur->next != nullptr && cur->next->x_min < edge->x_min) {
                // iterate to edge we want to insert after
                cur = cur->next;
            }
            // insert edge after cur
            Edge* next = cur->next;
            cur->next = edge;
            edge->next = next;
        }
    }

    return 0;
}

Edge* EdgeTable::RemoveEdge(int scanline) {
    // Remove the head from the scanline
    std::map<int,Edge*>::iterator iter;

    iter = this->scanlines.find(scanline);
    if (iter == this->scanlines.end()) {
        return nullptr;
    }
    else {
        Edge* head = iter->second;
        if (head->next == nullptr) {
            scanlines.erase(scanline);
        }
        else {
            this->scanlines[scanline] = head->next;
        }
        head->next = nullptr;
        return head;
    }
}

EdgeTable::~EdgeTable() {
    // Destructor, deletes all edges
    std::map<int,Edge*>::iterator it;
    for (it=this->scanlines.begin(); it != this->scanlines.end(); it++) {
        Edge* cur = it->second;
        while(cur != nullptr) {
            Edge* next = cur->next;
            delete cur;
            cur = next;
        }
    }
}

bool EdgeTable::IsEmpty() {
    return this->scanlines.empty();
}

void EdgeTable::PrintEdgeTable() {
    std::map<int,Edge*>::iterator it;
    for (it=this->scanlines.begin(); it != this->scanlines.end(); it++) {
        printf("Scanline %d\t: ", it->first);
        Edge* cur = it->second;
        while(cur != nullptr) {
            printf("(y_max=%d x_min=%f 1/m=%f z_min=%f delz=%f) ",cur->y_max, cur->x_min, cur->inv_m, cur->z_min, cur->del_z);
            cur = cur->next;
        }
        printf("\n");
    }
}

//=============================================
// Active Edge Table
//=============================================

ActiveEdgeTable::ActiveEdgeTable() {
    this->aet = new std::multimap<int,Edge*>();
}

ActiveEdgeTable::~ActiveEdgeTable() {
    // Destructor, deletes all edges
    if (this->aet != nullptr) {
        std::multimap<int,Edge*>::iterator it;
        for (it=(*this->aet).begin(); it != (*this->aet).end(); it++) {
            Edge* cur = it->second;
            delete cur;
            (*this->aet).erase(it->first);
        }
        delete this->aet;
    }
}

int ActiveEdgeTable::InsertEdge(int x_int, Edge* edge) {
    assert(this->aet != nullptr);
    (*this->aet).insert(std::pair<int,Edge*>(x_int, edge));
    return 0;
}

bool ActiveEdgeTable::IsEmpty() {
    assert(this->aet != nullptr);
    return (*this->aet).empty();
}

void ActiveEdgeTable::UpdateEdges(int scanline) {
    assert(this->aet != nullptr);

    std::multimap<int,Edge*> *new_aet = new std::multimap<int,Edge*>();
    // Only save edges whose y_max > scanline
    std::multimap<int,Edge*>::iterator it;
    for (it=(*this->aet).begin(); it != (*this->aet).end(); it++) {
        Edge* cur = it->second;
        // Update edge and move it to new table
        if (cur->y_max > scanline + 1) {
            cur->x_min += cur->inv_m;
            cur->z_min += cur->del_z;
            int x_int = (int)round(cur->x_min);
            (*new_aet).insert(std::pair<int,Edge*>(x_int, cur));
        }
        else {
            delete cur;
        }
    }

    // Switch old AET to new AET
    delete this->aet;
    this->aet = new_aet;
}

void ActiveEdgeTable::PrintActiveEdgeTable() {
    std::multimap<int,Edge*>::iterator it;
    assert(this->aet != nullptr);
    printf("AET: \n");
    for (it=(*this->aet).begin(); it != (*this->aet).end(); it++) {
        Edge* cur = it->second;
        printf("[%d](y_max=%d x_min=%f 1/m=%f z_min=%f delz=%f)\n",it->first, cur->y_max, cur->x_min, cur->inv_m, cur->z_min, cur->del_z);
    }
}