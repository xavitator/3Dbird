
#include "terrain.hpp"
#include "variable.hpp"
#include <algorithm>

using namespace vcl;
using std::vector;


/**
 * @brief Evaluate 3D position of the terrain for any (u,v) \in [0,1]
 * 
 * @param u x \in [0,1] 
 * @param v y \in [0,1]
 * @param parameters paramètre de bruit de perlin 
 * @return vec3 position du terrain {x,y,z}
 */
vec3 evaluate_terrain(float u, float v, perlin_noise_parameters const& parameters)
{
    float z(0);
    float x=20 * (u-0.5f);
    float y= 20 * (v-0.5f);
    float const noise = noise_perlin({ u, v }, parameters.octave, parameters.persistency, parameters.frequency_gain);
    z += parameters.terrain_height * noise * (1 - u) * u * (1 - v) * v * 100 ;
    //z = noise2 * (1 - u) * u * (1 - v) * v * 50* par2.terrain_height;
    return {x,y,z};
}

/**
 * @brief Create a terrain object
 * 
 * @param parameters1 paramètre de bruit de perlin pour la génération de l'ile
 * @return mesh objet correspondant à une ile
 */
mesh create_terrain(perlin_noise_parameters const& parameters1)
{
    // Number of samples of the terrain is N x N
    const unsigned int N = 100;

    mesh terrain_ile; // temporary terrain storage (CPU only)
    terrain_ile.position.resize(N * N);
    terrain_ile.uv.resize(N * N);
    terrain_ile.color.resize(N * N);

    // Fill terrain geometry
    for(unsigned int ku=0; ku<N; ++ku)
    {
        for(unsigned int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            // Compute the local surface function
            vec3 const p = evaluate_terrain(u,v, parameters1);

            // Store vertex coordinates
            terrain_ile.position[kv + N * ku] = p;
            terrain_ile.uv[kv + N * ku] = { u * N / 5,v * N / 5 };
            if (((kv - 50) * (kv - 50) + (ku - 50) * (ku - 50) )*(1 + 0.125f * sin(kv * 50 + ku * 50)) > 1500) {
                
                terrain_ile.color[kv + N * ku] = vec3{ 1.0f,1.0f,1.00f };
                terrain_ile.uv[kv + N * ku] = {((float) u * N / 5 - floor( u * N / 5))/2.01f,v * N / 5 };
            }
            else {
                terrain_ile.color[kv + N * ku] = vec3{ 1.0f,1.0f,1.0f };
                terrain_ile.uv[kv + N * ku] = { ((float)u * N / 5 - floor(u * N / 5)) / 2.01f+0.501,v * N / 5 };
            }
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(size_t ku=0; ku<N-1; ++ku)
    {
        for(size_t kv=0; kv<N-1; ++kv)
        {
            const unsigned int idx = kv + N*ku; // current vertex offset

            const uint3 triangle_1 = {idx, idx+1+N, idx+1};
            const uint3 triangle_2 = {idx, idx+N, idx+1+N};

            terrain_ile.connectivity.push_back(triangle_1);
            terrain_ile.connectivity.push_back(triangle_2);
        }
    }

	terrain_ile.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)
    return terrain_ile;
}

/**
 * @brief Create a ocean object
 * 
 * @param parameters bruit de perlin
 * @return mesh objet correspondant à un ocean
 */
mesh create_ocean(perlin_noise_parameters const& parameters) {
    timer.update();
    const unsigned int N = taille_terrain;

    mesh terrain_ile; // temporary terrain storage (CPU only)
    terrain_ile.position.resize(N * N);
    terrain_ile.uv.resize(N * N);
    
    // Fill terrain geometry
    for (unsigned int ku = 0; ku < N; ++ku)
    {
        for (unsigned int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);
            
            float const noise = noise_perlin({ std::sin(timer.t*v_maree*0+v), std::cos(timer.t * v_maree*0 +u) }, parameters.octave , parameters.persistency , parameters.frequency_gain);

            // Compute the local surface function
        
            float x = taille_terrain* (u - 0.5f);
            float y = taille_terrain* (v - 0.5f);
            
            

            vec3 const p = { x, y,std::max(0.01f, parameters.terrain_height * noise + noise_perlin({ u, v }, parameters.octave, 0.3f, parameters.frequency_gain) - 0.65f) };

            // Store vertex coordinates

            // Store vertex coordinates
            terrain_ile.position[kv + N * ku] = p;
            terrain_ile.uv[kv + N * ku] = { u * N / 5,v * N / 5 };
        }
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for (size_t ku = 0; ku < N - 1; ++ku)
    {
        for (size_t kv = 0; kv < N - 1; ++kv)
        {
            const unsigned int idx = kv + N * ku; // current vertex offset

            const uint3 triangle_1 = { idx, idx + 1 + N, idx + 1 };
            const uint3 triangle_2 = { idx, idx + N, idx + 1 + N };

            terrain_ile.connectivity.push_back(triangle_1);
            terrain_ile.connectivity.push_back(triangle_2);
        }
    }

    terrain_ile.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)
    return terrain_ile;
}

/**
 * @brief Create a wall object
 * 
 * @return mesh objet correspondant aux murs sur les cotés
 */
mesh create_wall() {
   
    mesh terrain_ile; // temporary terrain storage (CPU only)
    terrain_ile.position.resize(2 * 2);
    terrain_ile.uv.resize(2 * 2);
    float t = (float)taille_terrain/ 2;
    vec3 p = { t ,t,0 };
    terrain_ile.position[0] = p;
    terrain_ile.position[1] = { -t,t,0 };
    terrain_ile.position[2] = { t,t, ceiling_height };
    terrain_ile.position[3] = { -t,t, ceiling_height };
    const uint3 triangle_1 = { 0,   1 , 2 };
    const uint3 triangle_2 = { 1,   2 , 3 };
    terrain_ile.connectivity.push_back(triangle_1);
    terrain_ile.connectivity.push_back(triangle_2);
    terrain_ile.fill_empty_field();
    terrain_ile.uv[0] = { 1.0f,0.0f };
    terrain_ile.uv[1] = { 2.0f,1.0f };
    terrain_ile.uv[2] = { 0.0f,1.0f };
    terrain_ile.uv[3] = { 1.0f,2.0f };
    return terrain_ile;
}

/**
 * @brief Génération des positions des arbres sur les iles
 * 
 * @param parameters bruit de perlin pour avoir des lieux aléatoires
 * @return vector<vec3> vecteur de position
 */
vector<vec3> generate_positions_on_terrain( perlin_noise_parameters const& parameters) {
    std::vector<vec3> a;
    int N = nb_arbres;
    int nb = 0;
    for (int i = 0; i < N; i++) {
        vec3 b;
        int c = 0;
        nb = 0;
        while (!c) {
            b = evaluate_terrain(rand_interval()*0.7f+0.15f, rand_interval()*0.7f+0.15f, parameters);
            c = true;
            for (size_t k = 0; k < a.size(); k++) {
                if ((float)a[k][0] - (float)b[0] < 0.8f&& (float)a[k][0] - (float)b[0]>-0.8f && (float)a[k][1] - (float)b[1] <0.8f && (float)a[k][1] - (float)b[1]>-0.8f) {
                    c = false;
                }
            }
            nb += 1;
            if(nb > limite_essai)
                break;
        }
        if(nb > limite_essai)
                break;
        a.push_back(b);
        
    }
    return a;
}

/**
 * @brief Génération des positions des iles sur l'océan
 * 
 * @return vector<vec3> vecteur de position
 */
vector<vec3> generate_positions_ile() {
    std::vector<vec3> a;
    int N = nb_iles;
    vec3 b;
    float FLOAT_MIN =-(float) taille_terrain/2+10.0f;
    float FLOAT_MAX = (float)taille_terrain / 2 - 10.0f;
    int nb = 0;
    for (int i = 0; i < N; i++) {
        int c = false;
        nb = 0;
        while (!c) {
            b = { FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN), FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN) , 0 };
            c = true;
            for (size_t k = 0; k < a.size(); k++) {
                if (std::abs(a[k][0] - b[0]) < 20 
                && std::abs(a[k][1] - b[1]) < 20) {
                    c = false;
                }
            }
            nb += 1;
            if(nb > limite_essai)
                break;
        }
        if(nb > limite_essai)
                break;
        a.push_back(b);
    }
    return a;
}

/**
 * @brief Générer un bruit de perlin différent pour chaque ile
 * 
 * @return perlin_noise_parameters nouveau bruit de perlin (borné pour ne pas avoir des iles trop bizarre)
 */
perlin_noise_parameters generate_alea_ile() {
    perlin_noise_parameters par2;
    float FLOAT_MIN = 1.7f;
    float FLOAT_MAX = 2.2f;
    par2.frequency_gain = FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN);

    FLOAT_MIN = 0.3f;
    FLOAT_MAX = 0.4f;
    par2.persistency = FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN);

    FLOAT_MIN = 0.4f;
    FLOAT_MAX = 0.6f;
    par2.terrain_height = 0.4f + rand_interval() * (FLOAT_MAX - FLOAT_MIN);
    return par2;
}

/**
 * @brief Générer un vecteur d'angle de rotation
 * 
 * @param N nombre d'angles à générer
 * @return vector<float> vecteur d'angle
 */
vector<float> generate_rotation(int N)
{
    vector<float> b;
    for (int k = 0; k < N; k++) {
        float FLOAT_MIN = 0.0f;
        float FLOAT_MAX = 3.14f;
        b.push_back(FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN));
    }
    return b;
}

/**
 * @brief Génération des positions des nuages dans l'air
 * 
 * @return vector<vec3> vecteur de position
 */
vector<vec3> generate_positions_clouds() {
    std::vector<vec3> a;
    int N = nb_cloud;
    int nb = 0;
    for (int i = 0; i < N; i++) {
        vec3 b;
        int c = false;
        float FLOAT_MIN = -(float) taille_terrain/2.0f;
        float FLOAT_MAX = (float) taille_terrain / 2.0f;
        float HEIGHT_MAX = ceiling_height;
        float HEIGHT_MIN = 5.0f;
        nb = 0;
        while (!c) {
            b = { FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN), FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN) ,  HEIGHT_MIN + rand_interval() * (HEIGHT_MAX - HEIGHT_MIN) };
            c = true;
            for (size_t k = 0; k < a.size(); k++) {
                if (a[k][0] - b[0] <5 && a[k][0] - b[0]>-5 && a[k][1] - b[1] <5 && a[k][1] - b[1]>-5) {
                    c = false;
                }
            }
            nb += 1;
            if(nb > limite_essai)
                break;
        }
        if(nb > limite_essai)
                break;
        a.push_back(b);
    }
    return a;
}

/**
 * @brief Génère les différents anneaux
 * 
 */
void generate_positions_ring()
{
    int N = nb_ring;
    for (int i = 0; i < N; i++) {
        vec3 b;
        bool c = false;
        Ring::get_position(ring_objects, b, c);
        if(!c){
            std::cout << ring_objects.size() << std::endl;
            return;
        }
        Ring::add_ring(ring_objects, b);
    }
    
}

/**
 * @brief Génération des positions des bateaux sur l'océan
 * 
 * @return vector<vec3> vecteur de position
 */
vector<vec3> generate_positions_ships() {
    std::vector<vec3> a;
    int N = nb_ship;
    int nb = 0;
    for (int i = 0; i < N; i++) {
        vec3 b;
        int c = false;
        float FLOAT_MIN = -(float)taille_terrain / 2;
        float FLOAT_MAX = (float)taille_terrain / 2;
        float taille_ship = 8.0f;
        nb = 0;
        while (!c) {
            b = { FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN), FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN) , 0.35f };
            c = true;
            for (size_t k = 0; k < a.size(); k++) {
                if (a[k][0] - b[0] <taille_ship && a[k][0] - b[0]>-taille_ship && a[k][1] - b[1] <taille_ship && a[k][1] - b[1]>-taille_ship) {
                    c = false;
                }
            }
            for (size_t k = 0; k < ile_position.size(); k++) {
                if (std::abs(ile_position[k][0] - b[0]) < 15 && std::abs(ile_position[k][1] - b[1]) <15 ) {
                    c = false;
                }
            }
            nb += 1;
            if(nb > limite_essai)
                break;
        }
        if(nb > limite_essai)
                break;
        a.push_back(b);
    }
    return a;
}

/**
 * @brief Mets à jour la position de chacun des points composants l'océan (pour simuler l'eau qui bouge et la marée)
 * 
 * @param ocean mesh dont on doit modifier les positions
 * @param ocean_visual mesh_drawable dont on doit modifier les positions
 * @param parameters paramètre de perlin de l'océan
 */
void update_ocean(mesh& ocean, mesh_drawable& ocean_visual, perlin_noise_parameters const& parameters)
{
    timer.update();

    // Number of samples in each direction (assuming a square grid)
    int const N = taille_terrain;

    // Recompute the new vertices
    for (int ku = 0; ku < N; ++ku) {
        for (int kv = 0; kv < N; ++kv) {

            int const idx = ku * N + kv;
            
            // Compute the Perlin noise
            // use the noise as height value
            ocean.position[idx].z = ocean_height(ku, kv, N, parameters);

    
            // use also the noise as color value
            ocean.color[idx] = vec3(0.75f, 0.75f, 0.75f) +0.7f* ( ocean_height(ku, kv, N, parameters) * vec3(1, 1, 1));
            
        }
    }

    // Update the normal of the mesh structure
    ocean.compute_normal();

    // Update step: Allows to update a mesh_drawable without creating a new one
    ocean_visual.update_position(ocean.position);
    ocean_visual.update_normal(ocean.normal);
    ocean_visual.update_color(ocean.color);

}

/**
 * @brief Hauteur de l'océan à la position (a,b) pour une taille d'ocean N et un paramètre de perlin `parameters`
 * 
 * @param a x
 * @param b y
 * @param N taille de l'ocean
 * @param parameters bruit de perlin
 * @return float hauteur de l'ocean
 */
float ocean_height(float a, float b, int N, perlin_noise_parameters const& parameters) {
    const float u = a / (N - 1.0f);
    const float v = b / (N - 1.0f);

    // Compute the Perlin noise
    float const noise = noise_perlin({ std::sin(timer.t * v_maree + v), std::cos(timer.t * v_maree + u) }, 4, std::min(parameters.persistency+ timer.t * 0.01f,0.7f), parameters.frequency_gain);
    //float const h = std::cos(timer.t * 5.0f + u*50+v*50) + std::sin(timer.t * 5.0f + v*50+u*50);
    //return std::max(0.01f,  (h+1)*0.01f);
    return std::max(0.01f, ((noise-1)*0.1f+0.08f- std::min(parameters.persistency + timer.t * 0.01f, 0.7f)*0.08f)*10);
}

/**
 * @brief Déterminer la position suivante pour un nuage
 * 
 * @param position_initiale 
 * @return vcl::vec3 position suivante selon le vecteur vitesse donné
 */
vcl::vec3 cloud_deplacement(vcl::vec3 position_initiale)
{
    float v =0.2f;
    float dt = 0.1f;
    float a = ((float) v *(float) dt + (float) position_initiale[0]) ;
    while (a>taille_terrain/2)
    {
        a = a - taille_terrain;
    }
    position_initiale[0] = a;
    return position_initiale;
}

/**
 * @brief Regénérer le terrain en fonction des paramètres difinis
 * 
 */
void generate_terrain() {
    for(auto o : ring_objects)
        delete o;
    ring_objects.clear();
    generate_positions_ring();

    ile_position.clear();
    ile_orientation.clear();

    ile_position = generate_positions_ile();
    ile_orientation = generate_rotation(ile_position.size());

    ship_position.clear();
    ship_orientation.clear();
    ship_position = generate_positions_ships();
    ship_orientation = generate_rotation(ship_position.size());

    cloud_position.clear();
    cloud_orientation.clear();
    cloud_position = generate_positions_clouds();
    cloud_orientation = generate_rotation(cloud_position.size());

    mesh tmp = create_wall();
    wall.update_position(tmp.position);

    ocean.clear();
    ocean_m = create_ocean(parameters);
	ocean = mesh_drawable(ocean_m);
	//ocean.shading.color = { 0.0f, 0.0f, 1.0f };
	image_raw const im4 = image_load_png("assets/ocean.png");
	GLuint const texture_image_id4 = opengl_texture_to_gpu(im4,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);
	ocean.texture = texture_image_id4;

    liste_iles.clear();
    liste_tree_position.clear();
    liste_noise_ile.clear();
    for (int k = 0; k < ile_position.size(); k++) {
        perlin_noise_parameters par2 = generate_alea_ile();
        liste_iles.push_back(ile_g(par2));
        liste_tree_position.push_back(generate_positions_on_terrain(par2));
        liste_noise_ile.push_back(par2);
    }
    ile_position_inf = generate_positions_ile_inf(10);

}

/**
 * @brief Retourne une ile générée par les paramètres de perlin donnés
 * 
 * @param par2 bruit de perlin
 * @return mesh_drawable ile
 */
mesh_drawable ile_g(perlin_noise_parameters par2) {
	terrain = create_terrain(par2);
	mesh_drawable terrain_visual1 = mesh_drawable(terrain);
	//terrain_visual1.shading.color = { 0.6f,0.85f,0.5f };
	terrain_visual1.shading.phong.specular = 0.0f; // non-specular terrain material

	image_raw const im2 = image_load_png("assets/sandgrass.png");

	// Send this image to the GPU, and get its identifier texture_image_id
	GLuint const texture_image_id2 = opengl_texture_to_gpu(im2,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);

	terrain_visual1.texture = texture_image_id2;

	return terrain_visual1;
}

/**
 * @brief Create a ocean infini object : océan qu'on peut voir au loin
 * 
 * @return mesh océan à l'infini
 */
mesh create_ocean_infini() {
    mesh terrain_ile; // temporary terrain_ile storage (CPU only)
    terrain_ile.position.resize(2 * 2);
    terrain_ile.uv.resize(2 * 2);
    float t = (float)taille_terrain* 2;
    vec3 p = { t ,t,0 };
    terrain_ile.position[0] = p;
    terrain_ile.position[1] = { -t,t,0 };
    terrain_ile.position[2] = { -t,-t,0 };
    terrain_ile.position[3] = { t,-t,0 };
    const uint3 triangle_1 = { 0,   1 , 2 };
    const uint3 triangle_2 = { 2,   3 , 0 };
    terrain_ile.connectivity.push_back(triangle_1);
    terrain_ile.connectivity.push_back(triangle_2);
    terrain_ile.fill_empty_field();
    terrain_ile.uv[0] = { 100.0f,100.0f };
    terrain_ile.uv[1] = { -100.0f,100.0f };
    terrain_ile.uv[2] = { -100.0f,-100.0f };
    terrain_ile.uv[3] = { 100.0f,-100.0f };
    return terrain_ile;
}

/**
 * @brief Génération d'ile qui seront mis à l'infini pour le rendu visuel
 * 
 * @param N nombre d'ile à l'infini
 * @return vector<vec3> position de ces iles
 */
vector<vec3> generate_positions_ile_inf(int N) {
    std::vector<vec3> a;

    vec3 b;
    float FLOAT_MIN = -(float)taille_terrain + 10.0f;
    float FLOAT_MAX = (float)taille_terrain - 10.0f;
    int nb = 0;
    for (int i = 0; i < N; i++) {
        int c = false;
        nb = 0;
        while (!c) {
            b = { FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN), FLOAT_MIN + rand_interval() * (FLOAT_MAX - FLOAT_MIN) , 0 };
            c = true;
            //std::cout << b << "; " << a.size() << std::endl;
            for (size_t k = 0; k < a.size(); k++) {
                if (std::abs(a[k][0] - b[0]) < 20
                    && std::abs(a[k][1] - b[1]) < 20) {
                    c = false;
                }
            }
            if (std::abs(b[0]) < taille_terrain/2 + 10 && std::abs(b[1]) < taille_terrain/2 + 10) { 
                c = false; 
                
            }
            nb++;
            if(nb > limite_essai)
                break;
        }
        if(nb > limite_essai)
                break;
        a.push_back(b);
        
    }
    return a;
}