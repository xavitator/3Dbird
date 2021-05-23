#include "variable.hpp"
#include "hitbox.hpp"
#include <algorithm>
#include <bird.hpp>
#include <terrain.hpp>

#include "vcl/vcl.hpp"

using namespace vcl;
using std::vector;

int hit_oiseau() {
	vec3 pos = get_pos_bird();
	// test îles :
	//std::cout << "position oiseau1:" << std::endl;
	//std::cout << pos << std::endl;
	std::cout << "liste ile:" << std::endl;
	std::cout << ile_position[0] << std::endl;
	std::cout << ile_position[1] << std::endl;
	std::cout << ile_position[2] << std::endl;
	std::cout << ile_position[3] << std::endl;
	std::cout << ile_position[4] << std::endl;
	int indice_ile = get_plus_proche(pos, ile_position, 10);
	std::cout << "indice_ile" << std::endl;
	std::cout << indice_ile<< std::endl;
	
	if (pos[2] < 4.0f) {
		if (indice_ile != -1) {
			// position par rapport à l'ile
			float u = ((float)pos[0] - ile_position[indice_ile][0]) / 20 + 0.5f;
			std::cout << "u"<< std::endl;
			std::cout << u << std::endl;
			float v = ((float)pos[1] - ile_position[indice_ile][1]) / 20 + 0.5f;
			std::cout << "v" << std::endl;
			std::cout << v << std::endl;
			if (!(u > 1 || u < 0 || v>1 || v < 0)) {
				vec3 a = evaluate_terrain(u, v, liste_noise_ile[indice_ile]);
				std::cout << "a" << std::endl;
				std::cout << a << std::endl;
				if (pos[2] < a[2] +0.1f) {
					return 1;
				}
			}
			int indice_arbre = get_plus_proche(pos, liste_tree_position[indice_ile], 2);
			if (indice_arbre != -1) {
				vec3 pos_arbre = liste_tree_position[indice_ile][indice_arbre];
				if (norm(pos - ile_position[indice_ile] - pos_arbre) < 3.0f) {
					return 2;
				}
			}
		}
		// test ocean
		int indice_ship = get_plus_proche(pos, ship_position, 4);
		if (indice_ship != -1) {
			vec3 pos_ship = ship_position[indice_ship];
			if (norm(pos - pos_ship) < 4) {
				return 3;
			}
		}
		float z = ocean_height(pos[0], pos[1], taille_terrain, parameters, 0.2f);

		std::cout << z << std::endl;
		if (z > pos[2]-0.4f) {
			return 4;
		}
		
	}
	else {
		int indice_cloud = get_plus_proche(pos, cloud_position, 10);
		std::cout << "indice_cloud" << std::endl;
		std::cout << indice_cloud << std::endl;
		//std::cout << pos << std::endl;
		if (indice_cloud != -1) {
			if (norm(cloud_position[indice_cloud] - pos) < 5.0f) {
				return 5;
			}
		}
	}
	return -1;
}

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
		//std::cout << b << std::endl;
		while (a < b) {
			c =(a + b) / 2;
			//std::cout << c << std::endl;
			if (liste[c][k] > (float)pos[k] - taille_obj) {
				b = c;
			}
			else {
				a = c + 1;
				//std::cout << "a" << std::endl;
			}
		}
		//std::cout << "position objet:"<< std::endl;
		//std::cout << liste[a][k] << std::endl;
		//std::cout << "position oiseau:" << std::endl;
		//std::cout << pos << std::endl;
		//std::cout << pos[k] + taille_obj << std::endl;
		if (liste[a][k] > pos[k] + taille_obj) {
			return -1;
		}
		min = a;
		//std::cout << "min" << std::endl;
		b = liste.size() - 1;
		while (a < b) {
			int c = (a + b) / 2 + 1;
			if (liste[c][k] < pos[k] + taille_obj) {
				a = c ;
				
			}
			else {
				b = c-1;
				//std::cout << "bbbbb" << std::endl;
			}
			//std::cout << "c" << std::endl;
			//std::cout << a << std::endl;
			//std::cout << b << std::endl;
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
	//std::cout << "index" << std::endl;
	//std::cout << index << std::endl;
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
	vec3 a = evaluate_terrain(u, v, liste_noise_ile[k]);
	if (a[2] > pos[2] - 0.1f) return 1;
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
		if (vcl::abs(x1) < 5 && vcl::abs(x2) < 2 && pos[2] < 3.5f) return 1;
	}
	return -1;
}

int hit_ois() {
	vec3 pos = get_pos_bird();
	if (vcl::abs(pos[0]) > taille_terrain / 2 || vcl::abs(pos[1]) > taille_terrain / 2 || pos[2] > 11) return 6;
	
	if (pos[2] < 4) {
		int indice_ile = on_ile();
		
		if (indice_ile > -1) {
			if (hit_ile(indice_ile) > -1) return 1;
			if (hit_arbre(indice_ile) > -1) return 2;
		}
		else {
			float z = ocean_height(pos[0], pos[1], taille_terrain, parameters, 0.2f);
			if (z > pos[2] - 0.1f) return 3;
			if (hit_ship() > -1) return 4;
		}
	}
	else {
		if (hit_cloud() > -1)  return 5;
	}
	if (hit_cercle()>-1) return 7;
	return -1;
}

int hit_cercle() {
	{
		vec3 pos = get_pos_bird();
		for (int j = 0; j < ring_position.size(); j++) {
			vec3 b = ring_position[j];
			float theta = ring_orientation[j];
			float x1 = cos(theta) * (pos[0] - ring_position[j][0]) + sin(theta) * (pos[1] - ring_position[j][1]);
			float x2 = -sin(theta) * (pos[0] - ring_position[j][0]) + cos(theta) * (pos[1] - ring_position[j][1]);

			std::cout << "b[2] - pos[2]" << std::endl;
			std::cout << x1 << std::endl;
			std::cout << x2 << std::endl;
			std::cout << x1*x1+(b[2]-pos[2])* (b[2] - pos[2]) << std::endl;
			if (x1*x1+(b[2]-pos[2])* (b[2] - pos[2])<1.0f && vcl::abs(x2)<0.1f) return 1;
		}
		return -1;
	}
}