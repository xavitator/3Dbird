
#include "terrain.hpp"

using namespace vcl;
using std::vector;

// Evaluate 3D position of the terrain for any (u,v) \in [0,1]
vec3 evaluate_terrain(float u, float v, perlin_noise_parameters const& parameters)
{
    float p[4][2];
    p[0][0] = 0.0f;
    p[0][1] = 0.0f;
    p[1][0] = 0.5f;
    p[1][1] = 0.5f;
    p[2][0] = 0.2f;
    p[2][1] = 0.7f;
    p[3][0] = 0.8f;
    p[3][1] = 0.7f;

    float h[4];
    h[0] = 3.0f;
    h[1] = -1.5f;
    h[2] = 1;
    h[3] = 2;

    float sigma[4];
    sigma[0] = 0.5f;
    sigma[1] = 0.15f;
    sigma[2] = 0.2f;
    sigma[3] = 0.2f;

    float z(0);
    float x=20 * (u-0.5f);
    float y= 20 * (v-0.5f);

    for (int i(0); i < 4; i++) {
        vec2 const u0 = { p[i][0], p[i][1] };
        float const h0 = h[i];
        float const sigma0 = sigma[i];

        float const noise = noise_perlin({ u, v }, parameters.octave, parameters.persistency, parameters.frequency_gain);

        float const d = norm(vec2(u, v) - u0) / sigma0;
        z += h0 * std::exp(-d * d) + parameters.terrain_height * noise;
    }
 

    return {x,y,z};
}

mesh create_terrain(perlin_noise_parameters const& parameters)
{
    // Number of samples of the terrain is N x N
    const unsigned int N = 100;

    mesh terrain; // temporary terrain storage (CPU only)
    terrain.position.resize(N * N);
    terrain.uv.resize(N * N);

    // Fill terrain geometry
    for(unsigned int ku=0; ku<N; ++ku)
    {
        for(unsigned int kv=0; kv<N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku/(N-1.0f);
            const float v = kv/(N-1.0f);

            float const noise = noise_perlin({ u, v }, parameters.octave, parameters.persistency, parameters.frequency_gain);

            // Compute the local surface function
            vec3 const p = evaluate_terrain(u,v, parameters);

            // Store vertex coordinates
            terrain.position[kv + N * ku] = p;
            terrain.uv[kv + N * ku] = { u * N / 5,v * N / 5 };
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

            terrain.connectivity.push_back(triangle_1);
            terrain.connectivity.push_back(triangle_2);
        }
    }

	terrain.fill_empty_field(); // need to call this function to fill the other buffer with default values (normal, color, etc)
    return terrain;
}

vector<vec3> generate_positions_on_terrain(int N, perlin_noise_parameters const& parameters) {
    std::vector<vec3> a;
    
    for (int i = 0; i < N; i++) {
        vec3 b;
        int c = 0;
        while (!c) {
            b = evaluate_terrain(rand_interval(), rand_interval(), parameters);
            c = 1;
            for (size_t k = 0; k < a.size(); k++) {
                if (a[k][0] - b[0] < 0.8&& a[k][0] - b[0]>-0.8 && a[k][1] - b[1] <0.8 && a[k][1] - b[1]>-0.8) {
                    c = 0;
                }
            }
        }
        a.push_back(b);
        
    }
    return a;
}

