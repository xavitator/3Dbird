#pragma once

#include <iostream>
#include <list>
#include "vcl/vcl.hpp"

using namespace vcl;


class Ring{

    public :
        vec3 position;
        float orientation;
        vec3 color;

        Ring(vec3 _position);

        virtual void cross();
        static void get_position(std::vector<Ring *> _list_ring, vec3 &_position, bool &exist);
        static void add_ring(std::vector<Ring *> &_list_ring, vec3 &_position);
};

class accelerationRing : public Ring {

    public : 
        accelerationRing(vec3 _position);

        void cross();
};

class decelerationRing : public Ring {

    public : 
        decelerationRing(vec3 _position);

        void cross();
};

class goldenRing : public Ring {

    public : 
        goldenRing(vec3 _position);

        void cross();
};

class deadRing : public Ring {

    public : 
        deadRing(vec3 _position);

        void cross();
};