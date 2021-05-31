#include "variable.hpp"
#include "hitbox.hpp"
#include <algorithm>
#include <bird.hpp>
#include <terrain.hpp>

#include "vcl/vcl.hpp"

using namespace vcl;
using std::vector;

/**
 * @brief Retourne l'indice de l'île en (x,y) ou -1 si aucune
 * 
 * @param x 
 * @param y 
 * @return int indice de l'ile dans le tableau
 */
int on_ile(float x, float y) {
	for (int k = 0; k < ile_position.size(); k++) {
		if (vcl::abs(x - ile_position[k][0]) < 10 && vcl::abs(y - ile_position[k][1]) < 10) {
			return k;
		}
	}
	return -1;
}

/**
 * @brief Retourne la hauteur de l'île en (x,y) ou -1 si pas au dessus de l'ile
 * 
 * @param x 
 * @param y 
 * @return float hauteur de l'ile au point (x,y)
 */
float height_ile(float x, float y) {
	int k = on_ile(x,y);
	if(k < 0) return -1;
	float u = ((float)x - ile_position[k][0]) / 20 + 0.5f;
	float v = ((float)y - ile_position[k][1]) / 20 + 0.5f;
	vec3 a = evaluate_terrain(u, v, liste_noise_ile[k]);
	return a[2];
}

/**
 * @brief Hauteur de l'ocean au point (x,y)
 * 
 * @param x 
 * @param y 
 * @return float Hauteur
 */
float height_ocean(float x, float y) {
	float z = ocean_height(x+taille_terrain/2, y + taille_terrain / 2, taille_terrain, parameters);
	return z;
}

/**
 * @brief Retourne l'indice de l'ile sur laquelle est l'oiseau
 * 
 * @return int indice de l'ile, -1 si pas d'ile
 */
int on_ile() {
	vec3 pos = get_pos_bird();
	return on_ile(pos[0], pos[1]);
}

/**
 * @brief Retourne si l'oiseau heurte une ile
 * 
 * @return int 1 oui, 0 non
 */
int hit_ile() {
	vec3 pos = get_pos_bird();
	float h_i = height_ile(pos[0], pos[1]);
	if (h_i > pos[2] - 0.15f) return true;
	return false;
}

/**
 * @brief Retourne si l'oiseau heurte l'arbre k
 * 
 * @param k indice de l'arbre
 * @return int 1 oui, 0 non
 */
int hit_arbre(int k) {
	vec3 pos = get_pos_bird();
	for (int j = 0; j < liste_tree_position[k].size(); j++) {
		vec3 b = liste_tree_position[k][j];
		
		if (vcl::abs(b[0] - ((float)pos[0] - ile_position[k][0])) < 1.0f &&
			vcl::abs(b[1] - (float)pos[1] + ile_position[k][1]) < 1.0f &&
			b[2] + 1.0f > pos[2])
			return 1;
	}
	return 0;
}

/**
 * @brief Retourne si l'oiseau heurte un nuage
 * 
 * @return int 1 oui, 0 non
 */
int hit_cloud() {
	vec3 pos = get_pos_bird();
	for (int j = 0; j < cloud_position.size(); j++) {
		vec3 b = cloud_position[j];
		if (norm(b - pos) < 2) return 1;
	}
	return 0;
}

/**
 * @brief Retourne si l'oiseau heurte un bateau
 * 
 * @return int 1 oui, 0 non
 */
int hit_ship() {
	vec3 pos = get_pos_bird();
	float x = pos[0];
	float y = pos[1];
	
	for (int j = 0; j < ship_position.size(); j++) {
		float theta = ship_orientation[j];
		float x1 = cos(theta) * (x - ship_position[j][0]) + sin(theta) * (y - ship_position[j][1]);
		float x2 = -sin(theta) * (x - ship_position[j][0]) + cos(theta) * (y - ship_position[j][1]);
		if (vcl::abs(x1) < 2.5f && vcl::abs(x2) < 1.5f && (pos[2] - ship_position[j][2]) < 3.0f) {
			
			return 1; 
		}
		if (vcl::abs(x1 - 0.2f) < 0.7f && vcl::abs(x2) < 1.5f && (pos[2]- ship_position[j][2]) < 6.0f) {
			return 1;
		}
	}
	return 0;
}

/**
 * @brief Retourne l'indice de l'anneau que heurte l'oiseau
 * 
 * @return int indice de l'anneau
 */
int hit_ring() {
	{
		vec3 pos = get_pos_bird();
		for (int j = 0; j < ring_objects.size(); j++) {
			Ring b = *ring_objects[j];
			float theta = b.orientation;
			vec3 ring_pos = b.position;
			float x1 = cos(theta) * (pos[0] - ring_pos[0]) + sin(theta) * (pos[1] - ring_pos[1]);
			float x2 = -sin(theta) * (pos[0] - ring_pos[0]) + cos(theta) * (pos[1] - ring_pos[1]);
			if (x1*x1+(ring_pos[2]-pos[2])* (ring_pos[2] - pos[2])<1.0f && vcl::abs(x2)<0.1f) return j;
		}
		return -1;
	}
}

/**
 * @brief Retourne si l'oiseau heurte un anneau
 * 
 * @return int 1 oui, 0 non
 */
int hit_cercle() {
	{
		if(hit_ring() < 0) return 0;
		return 1;
	}
}

/**
 * @brief Retourne le type de choc que fait l'oiseau
 * 
 * @return int -1 si rien, et l'indice de l'enum Collision si oui
 */
int hit_ois() {
	vec3 pos = get_pos_bird();
	if (vcl::abs(pos[0]) > taille_terrain / 2 || vcl::abs(pos[1]) > taille_terrain / 2) return BORDER;
	if (pos[2] > ceiling_height) return CEILING;
	if (pos[2] < 4) {
		int indice_ile = on_ile();
		
		if (indice_ile > -1) {
			if (hit_ile()) return ILE;
			if (hit_arbre(indice_ile)) return TREE_ROCK;
		}
		else {
			float z = height_ocean(pos[0], pos[1]);
			if (z > pos[2] - 0.1f) return OCEAN;
			if (hit_ship()) return SHIP;
		}
	}
	else {
		if (hit_cloud())  return CLOUD;
	}
	if (hit_cercle()) return RING;
	return -1;
}