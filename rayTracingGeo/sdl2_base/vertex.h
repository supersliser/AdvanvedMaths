#pragma once

#include "point.h"
#include "geo.h"

class point;
class face;

class vertex : public point {
    private:
        vertex** connections;
        face** faces;
        int faceCount;
        int connectionCount;

    public:
        vertex();
        vertex(float x, float y, float z);
        void addConnection(vertex* v);
        void removeConnection(vertex* v);
        vertex** getConnections();
        int getConnectionIndex(vertex* v);
        vertex* getConnection(int i);
        int getConnectionCount();
        int getFaceCount();

        void addFace(face* f);
        void removeFace(face* f);

        face** getFaces();
        int getFaceIndex(face* f);
        face* getFace(int i);

        bool operator==(vertex v);
        bool operator!=(vertex v);

        void printVertex();

        point getPos();
};