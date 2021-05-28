#include "variable.hpp"
#include "hitbox.hpp"
#include <algorithm>
#include <bird.hpp>
#include <terrain.hpp>

#include "vcl/vcl.hpp"

using namespace vcl;
using std::vector;

int get_plus_proche(vec3 pos, vector<vec3> liste, int taille_obj) {
	int min = 0;
	int max = liste.size()-1;
	
	vec3 plus_proche;
	int index;
	int a;
	int b;
	int c;
	for (int k = 0; k < 3; k++) {
		a = min;
		b = max;
		while (a < b) {
			c =(a + b) / 2;
			if (liste[c][k] > (float)pos[k] - taille_obj) {
				b = c;
			}
			else {
				a = c + 1;
			}
		}
		if (liste[a][k] > pos[k] + taille_obj) {
			return -1;
		}
		min = a;
		b = liste.size() - 1;
		while (a < b) {
			int c = (a + b) / 2 + 1;
			if (liste[c][k] < pos[k] + taille_obj) {
				a = c ;
				
			}
			else {
				b = c-1;
			}
		}
		max = b;
	}
	plus_proche = liste[min];
	float norme = norm(plus_proche - pos);
	index = min;
	for (int k = min + 1; k < max + 1; k++) {
		float norme2 = norm(pos - liste[k]);
		if (norme2 < norme) {
			norme = norme2;
			plus_proche = liste[k];
			index = k;
		}
	}
	return index;
}

int on_ile() {
	vec3 pos = get_pos_bird();
	for (int k = 0; k < ile_position.size(); k++) {
		if (vcl::abs(pos[0] - ile_position[k][0]) < 10 && vcl::abs(pos[1] - ile_position[k][1]) < 10) {
			return k;
		}
	}
	return -1;
}

int hit_ile(int k) {
	vec3 pos = get_pos_bird();
	float u = ((float)pos[0] - ile_position[k][0]) / 20 + 0.5f;
	float v = ((float)pos[1] - ile_position[k][1]) / 20 + 0.5f;
	std::cout << u << ";" << v << endl;
	vec3 a = evaluate_terrain(u, v, liste_noise_ile[k]);

	std::cout << pos[2] << ";" << a[2] << endl;
	if (a[2] > pos[2] - 0.15f) return 1;
	return -1;
}

int hit_arbre(int k) {
	vec3 pos = get_pos_bird();
	float u = ((float)pos[0] - ile_position[k][0]) / 20 + 0.5f;
	float v = ((float)pos[1] - ile_position[k][1]) / 20 + 0.5f;
	vec3 a = evaluate_terrain(u, v, liste_noise_ile[k]);
	for (int j = 0; j < liste_tree_position[k].size(); j++) {
		vec3 b = liste_tree_position[k][j];
		
		if (vcl::abs(b[0] - ((float)pos[0] - ile_position[k][0])) < 1.0f &&
			vcl::abs(b[1] - (float)pos[1] + ile_position[k][1]) < 1.0f &&
			b[2] + 1.0f > pos[2])
			return 1;
	}
	return -1;
}

int hit_cloud() {
	vec3 pos = get_pos_bird();
	for (int j = 0; j < cloud_position.size(); j++) {
		vec3 b = cloud_position[j];
		if (norm(b - pos) < 2) return 1;
	}
	return -1;
}

int hit_ship() {
	vec3 pos = get_pos_bird();
	float x = pos[0];
	float y = pos[1];
	
	for (int j = 0; j < ship_position.size(); j++) {
		float theta = ship_orientation[j];
		float x1 = cos(theta) * (x - ship_position[j][0]) + sin(theta) * (y - ship_position[j][1]);
		float x2 = -sin(theta) * (x - ship_position[j][0]) + cos(theta) * (y - ship_position[j][1]);
		if (vcl::abs(x1) < 2.5f && vcl::abs(x2) < 1.5f && (pos[2] - ship_position[j][2]) < 3.0f) {
			
			return 1; }
		if (vcl::abs(x1 - 0.2f) < 0.7f && vcl::abs(x2) < 1.5f && (pos[2]- ship_position[j][2]) < 6.0f) {
			std::cout << ship_position[j][2]<<";" << pos[2] << std::endl;
			return 1;
		}
	}
	return -1;
}

int hit_ring() {
	{
		vec3 pos = get_pos_bird();
		for (int j = 0; j < ring_objects.size(); j++) {
			Ring b = ring_objects[j];
			float theta = b.orientation;
			vec3 ring_pos = b.position;
			float x1 = cos(theta) * (pos[0] - ring_pos[0]) + sin(theta) * (pos[1] - ring_pos[1]);
			float x2 = -sin(theta) * (pos[0] - ring_pos[0]) + cos(theta) * (pos[1] - ring_pos[1]);
			if (x1*x1+(ring_pos[2]-pos[2])* (ring_pos[2] - pos[2])<1.0f && vcl::abs(x2)<0.1f) return j;
		}
		return -1;
	}
}

int hit_cercle() {
	{
		if(hit_ring() < 0) return -1;
		return 1;
	}
}

int hit_ois() {
	vec3 pos = get_pos_bird();
	if (vcl::abs(pos[0]) > taille_terrain / 2 || vcl::abs(pos[1]) > taille_terrain / 2) return BORDER;
	if (pos[2] > ceiling_height) return CEILING;
	if (pos[2] < 4) {
		int indice_ile = on_ile();
		
		if (indice_ile > -1) {
			if (hit_ile(indice_ile) > -1) return ILE;
			if (hit_arbre(indice_ile) > -1) return TREE_ROCK;
		}
		else {
			float z = ocean_height(pos[0]+taille_terrain/2, pos[1] + taille_terrain / 2, taille_terrain, parameters);
			std::cout << z <<";"<< pos[2]<< std::endl;
			if (z > pos[2] - 0.1f) return OCEAN;
			if (hit_ship() > -1) return SHIP;
		}
	}
	else {
		if (hit_cloud() > -1)  return CLOUD;
	}
	if (hit_cercle()>-1) return RING;
	return -1;
}