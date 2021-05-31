#include "tree.hpp"

using namespace vcl;

/**
 * @brief Create a tree trunk cylinder object 
 * 
 * @param radius radius du tronc
 * @param height taille du tronc
 * @return mesh mesh représentant le tronc
 */
mesh create_tree_trunk_cylinder(float radius, float height)
{
 

    float const r = radius;
    float const h = height;

    // Number of samples of the terrain is N x N
    const unsigned int N = 20;

    mesh cylinder; // temporary terrain storage (CPU only)
    cylinder.position.resize(N * N);
    cylinder.uv.resize(N * N);

    // Fill terrain geometry
    for (unsigned int ku = 0; ku < N; ++ku)
    {
        for (unsigned int kv = 0; kv < N; ++kv)
        {
            // Compute local parametric coordinates (u,v) \in [0,1]
            const float u = ku / (N - 1.0f);
            const float v = kv / (N - 1.0f);

            // Compute the local surface function
            vec3 const p = { r * std::cos(2 * pi * u), r * std::sin(2 * pi * u), h * (v) };

            // Store vertex coordinates
            cylinder.position[kv + N * ku] = p;
            cylinder.uv[kv + N * ku] = { u * N / 10,v * N / 10 };
        }
    }

    // Generate triangle organization
    for (size_t ku = 0; ku < N - 1; ++ku)
    {
        for (size_t kv = 0; kv < N - 1; ++kv)
        {
            const unsigned int idx = kv + N * ku;

            const uint3 triangle_1 = { idx, idx + 1 + N, idx + 1 };
            const uint3 triangle_2 = { idx, idx + N, idx + 1 + N };

            cylinder.connectivity.push_back(triangle_1);
            cylinder.connectivity.push_back(triangle_2);
        }
    }

    cylinder.fill_empty_field();
    
    return cylinder;
}



/**
 * @brief Create a cone object
 * 
 * @param radius Radius du cone
 * @param height taille du cone
 * @param z_offset offset selon z du cone
 * @return mesh 
 */
mesh create_cone(float radius, float height, float z_offset)
{
    mesh m; 
    const size_t N = 20;

    for (size_t k = 0; k < N; ++k)
    {
        const float u = k / float(N);
        const vec3 p = { radius * std::cos(2 * 3.14f * u), radius * std::sin(2 * 3.14f * u), z_offset };
        m.position.push_back(p);
    }
    m.position.push_back(vec3(0, 0, z_offset));
    m.position.push_back(vec3(0, 0, height + z_offset));
    // Connectivity
    for (size_t k = 0; k < N; ++k)
    {
      

        const uint3 t1 = { k, (k+1)%N, N };
        const uint3 t2 = { k, (k+1)%N, N+1 };
        m.connectivity.push_back(t1);
        m.connectivity.push_back(t2);
    }

    m.fill_empty_field();
    return m;
}

/**
 * @brief Create a tree object
 * 
 * @return mesh Renvoie un arbre
 */
mesh create_tree()
{
    float const h = 1.0f; // trunk height
    float const r = 0.1f; // trunk radius

    // Create a brown trunk
    mesh trunk = create_tree_trunk_cylinder(r, h);
    trunk.color.fill({ 0.4f, 0.3f, 0.3f });


    // Create a green foliage from 3 cones
    mesh foliage = create_cone(4 * r, 6 * r, 0.0f);      // base-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 2 * r));   // middle-cone
    foliage.push_back(create_cone(4 * r, 6 * r, 4 * r));   // top-cone
    foliage.position += vec3(0, 0, h);                 // place foliage at the top of the trunk
    foliage.color.fill({ 0.4f, 0.6f, 0.3f });

    // The tree is composted of the trunk and the foliage
    mesh tree = trunk;
    tree.push_back(foliage);

    return tree;
}