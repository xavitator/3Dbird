#include "variable.hpp"
#include "hitbox.hpp"
#include <algorithm>
#include <bird.hpp>
#include <terrain.hpp>

#include "vcl/vcl.hpp"

using namespace vcl;

/**
 * @brief Construct a new Ring:: Ring object
 * 
 * @param _position position de l'anneau
 */
Ring::Ring(vec3 _position) : position(_position), orientation(rand_interval() * pi), color(1.0f, 0, 0){}

/**
 * @brief Comportement lorsqu'un anneau est croisé
 * 
 */
void Ring::cross(){
    user.score += 10;
}

/**
 * @brief Choc entre un anneau et une position
 * 
 * @param o position du prochain anneau
 * @return true il y a choc
 * @return false pas de choc
 */
bool Ring::choc(vec3 o){
    for (size_t i = 0; i < 3; i++)
    {
        if(std::abs(position[i] - o[i]) > 5)
            return false;
    }
    return true;
}

/**
 * @brief Déterminer une position où on peut mettre un anneau. La position est stockée dans la variable `_position`. 
 * Pour savoir si une position existe, nous changeons la variable `exist`.
 * 
 * @param _list_ring liste des anneaux existants
 * @param _position position du prochain anneau
 * @param exist est ce que la variable `_position` a été changé.
 */
void Ring::get_position(std::vector<Ring*> _list_ring, vec3 &_position, bool &exist){
    int nb = 0;
    int c = false;
    float FLOAT_MIN = -(float)taille_terrain / 2.0f;
    float FLOAT_MAX = (float)taille_terrain / 2.0f;
    float HEIGHT_MAX = ceiling_height;
    float HEIGHT_MIN = 0.2f;
    while (!c) {
        _position[0] = FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN);
        _position[1] =  FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN);
        _position[2] = HEIGHT_MIN + rand_interval() * (HEIGHT_MAX - HEIGHT_MIN);
        c = true;
        for (size_t k = 0; k < _list_ring.size(); k++) {
            if (_list_ring[k]->choc(_position)) {
                c = false;
            }
        }
        nb += 1;
        if(nb > limite_essai)
            break;
    }
    if(nb > limite_essai)
        exist = false;
    else 
        exist = true;
}

/**
 * @brief Créer un anneau au hasard parmi ceux existant et l'ajoute dans la liste d'anneaux.
 * 
 * @param _list_ring liste d'anneaux
 * @param _position position du nouvel anneau
 */
void Ring::add_ring(std::vector<Ring *> &_list_ring, vec3 &_position){
    int alea = std::floor(rand_interval() * 21);
    if(alea == 0){
        _list_ring.push_back(new goldenRing(_position));
    }
    else if(alea == 1){
        _list_ring.push_back(new deadRing(_position));
    }
    else if(alea < 5){
        _list_ring.push_back(new decelerationRing(_position));
    }
    else if(alea < 9){
        _list_ring.push_back(new accelerationRing(_position));
    }
    else
        _list_ring.push_back(new Ring(_position));
}

accelerationRing::accelerationRing(vec3 _position) : Ring(_position) {
    color = vec3(0, 0, 1.0f);
}

void accelerationRing::cross(){
    user.speed += 0.001f;
    user.score += 50;
}

decelerationRing::decelerationRing(vec3 _position) : Ring(_position){
    color = vec3(0, 1.0f, 0);
}

void decelerationRing::cross(){
    user.speed = std::max(user.speed - 0.001f, 0.001f);
    user.score += 5;
}

goldenRing::goldenRing(vec3 _position) : Ring(_position){
    color = vec3(0.921f, 0.776f, 0.258f);
}

void goldenRing::cross(){
    user.score += 100;
}

deadRing::deadRing(vec3 _position) : Ring(_position){
    color = vec3(0, 0, 0);
}

void deadRing::cross(){
    user.dead = true;
}