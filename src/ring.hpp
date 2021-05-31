#pragma once

#include <iostream>
#include <list>
#include "vcl/vcl.hpp"

using namespace vcl;


/**
 * @brief Anneau classique
 * 
 */
class Ring{

    public :
        vec3 position;
        float orientation;
        vec3 color;

        Ring(vec3 _position);
        virtual ~Ring(){};

        bool choc(vec3 o);

        virtual void cross();
        static void get_position(std::vector<Ring *> _list_ring, vec3 &_position, bool &exist);
        static void add_ring(std::vector<Ring *> &_list_ring, vec3 &_position);
};

/**
 * @brief Anneau d'accélération
 * 
 */
class accelerationRing : public Ring {

    public : 
        accelerationRing(vec3 _position);

        void cross();
};

/**
 * @brief Anneau de déceleration
 * 
 */
class decelerationRing : public Ring {

    public : 
        decelerationRing(vec3 _position);

        void cross();
};

/**
 * @brief Anneau d'or (similaire au vif d'or)
 * 
 */
class goldenRing : public Ring {

    public : 
        goldenRing(vec3 _position);

        void cross();
};

/**
 * @brief Anneau de la mort
 * 
 */
class deadRing : public Ring {

    public : 
        deadRing(vec3 _position);

        void cross();
};