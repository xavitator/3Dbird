#include "vcl/vcl.hpp"
#include "variable.hpp"
#include <iostream>
#include <typeinfo>  //for 'typeid' to work  



using namespace vcl;

hierarchy_mesh_drawable hierarchy_bird;
vec3 orientation_bird = {0,1,0};
vec3 pos_without_oscill;

void opengl_uniform(GLuint shader, scene_environment const& current_scene);

mesh create_cone_bird(float radius, float height, float z_offset)
{
    mesh m; 
    
    const size_t N = 20;

    // Fill cone geometry
    const vec3 base = {0, 0, z_offset};
    m.position.push_back(base);
    const unsigned int ib = 0;
    const vec3 top = {0, 0, height + z_offset};
    m.position.push_back(top);
    const unsigned int it = 1;
    for(unsigned int ku=0; ku<N; ++ku)
    {
        float u = ku/float(N);
        const vec3 p = {radius * std::cos(2*3.14*u), radius * std::sin(2*3.14*u), z_offset};
        m.position.push_back(p);
    }

    // Generate triangle organization
    //  Parametric surface with uniform grid sampling: generate 2 triangles for each grid cell
    for(size_t ku=0; ku<N; ++ku)
    {
        const unsigned int u0 = ku+2;
        const unsigned int u1 = (ku + 1)%(N) + 2;

        const uint3 t1 = {u0, u1, ib};
        const uint3 t2 = {u0, u1, it};

        m.connectivity.push_back(t1);
        m.connectivity.push_back(t2);
    }

    m.fill_empty_field();
    return m;
}

void create_bird(){

	// Definition of the elements of the hierarchy_bird
	// ------------------------------------------- //

	float const radius_body = 0.25f;
    float const radius_arm = 0.05f;
    float const length_arm = 0.2f;
	float const radius_head = 2* radius_body/3;
	float const radius_noise = radius_head/2;;

    // The geometry of the body is a sphere
    mesh_drawable body = mesh_drawable( mesh_primitive_ellipsoid({radius_body, radius_body*2, radius_body}, {0,0,0}, 40, 40));

    mesh_drawable head = mesh_drawable( mesh_primitive_sphere(radius_head, {0,0,0}, 40, 40));

    //mesh_drawable noise = mesh_drawable( mesh_primitive_triangle({radius_noise, radius_noise*2, 0}, {0,0,0}, 40, 40));
    mesh_drawable noise = mesh_drawable( create_cone_bird(radius_noise, 2*radius_noise, 0) );
	noise.shading.color = {0.8f,0.5f,0};
	noise.transform.rotate =  rotation({1,0,0}, -3.14f/2 );

	// Geometry of the eyes: black spheres
	mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(0.03f, {0,0,0}, 20, 20));
    eye.shading.color = {0,0,0};

	// Shoulder part and arm are displayed as cylinder
    mesh_drawable shoulder_left = mesh_drawable(mesh_primitive_cylinder(radius_arm, {0,0,0}, {-length_arm,0,0}));
    mesh_drawable arm_left = mesh_drawable(mesh_primitive_cylinder(radius_arm, {0,0,0}, {-length_arm/1.5f,0,-length_arm/1.0f}));

	mesh_drawable shoulder_right = mesh_drawable(mesh_primitive_cylinder(radius_arm, {0,0,0}, {length_arm,0,0}));
    mesh_drawable arm_right = mesh_drawable(mesh_primitive_cylinder(radius_arm, {0,0,0}, {length_arm/1.5f,0,-length_arm/1.0f}));

	// An elbow displayed as a sphere
    mesh_drawable elbow = mesh_drawable(mesh_primitive_sphere(0.055f));


	// Build the hierarchy_bird:
	// ------------------------------------------- //
    // Syntax to add element
    //   hierarchy_bird.add(visual_element, element_name, parent_name, (opt)[translation, rotation])

	// The root of the hierarchy_bird is the body
	hierarchy_bird.add(body, "body");

	hierarchy_bird.add(head, "head", "body", {0,2*radius_body+radius_head - 0.05f,0.1f});

	// Eyes positions are set with respect to some ratio of the body
	hierarchy_bird.add(eye, "eye_left", "head" , radius_head * vec3( 1/3.0f, 1/2.0f, 1/1.5f));
	hierarchy_bird.add(eye, "eye_right", "head", radius_head * vec3(-1/3.0f, 1/2.0f, 1/1.5f));

	hierarchy_bird.add(noise, "noise", "head", radius_head * vec3( 0.0f, 1.0f, 0.0f) - vec3(0,0.05f,0));

	// Set the left part of the body arm: shoulder-elbow-arm
    hierarchy_bird.add(shoulder_left, "shoulder_left", "body", {-radius_body+0.05f,0,0}); // extremity of the spherical body
    hierarchy_bird.add(elbow, "elbow_left", "shoulder_left", {-length_arm,0,0});          // place the elbow the extremity of the "shoulder cylinder"
    hierarchy_bird.add(arm_left, "arm_bottom_left", "elbow_left");                        // the arm start at the center of the elbow

	// Set the right part of the body arm: similar to the left part with a symmetry in x direction
    hierarchy_bird.add(shoulder_right, "shoulder_right", "body", {radius_body-0.05f,0,0});
    hierarchy_bird.add(elbow, "elbow_right", "shoulder_right", {length_arm,0,0});
    hierarchy_bird.add(arm_right, "arm_bottom_right", "elbow_right");

    pos_without_oscill = hierarchy_bird["body"].transform.translate;
}

void move_bird(){
    /** *************************************************************  **/
    /** Compute the (animated) transformations applied to the elements **/
    /** *************************************************************  **/

    timer.update();
    float t = timer.t;
    float coef_rot = 0.01f;
    // The body oscillate along the z direction
    hierarchy_mesh_drawable_node bird = hierarchy_bird["body"];
    //hierarchy_bird["body"].transform.translate += {0,0,0.02f*(std::sin(2*3.14f*t))};
    mat3 tmp = hierarchy_bird["body"].transform.rotate.matrix();
    hierarchy_bird["body"].transform.translate += user.speed*coef_rot*t*tmp*orientation_bird;
    pos_without_oscill += user.speed*coef_rot*t*tmp*orientation_bird;

    vec3 rot_vec;
    float rot_facteur = 0.05f;
    rotation tmpp;
    int b = 0;
    if(user.keyboard_state.up){
        rot_vec = {1,0,0};
        b = 1;
        tmpp = hierarchy_bird["body"].transform.rotate;
    	tmpp = tmpp * rotation(rot_vec, rot_facteur);
        hierarchy_bird["body"].transform.rotate = tmpp;
    }
    if(user.keyboard_state.down){
        b = 1;
        rot_vec = {-1,0,0};
        tmpp = hierarchy_bird["body"].transform.rotate;
    	tmpp = tmpp * rotation(rot_vec, rot_facteur);
        hierarchy_bird["body"].transform.rotate = tmpp;
    }
    if(user.keyboard_state.right){
        b = 1;
        rot_vec = {0,0,-1};
    	tmpp = hierarchy_bird["body"].transform.rotate;
    	tmpp = tmpp * rotation(rot_vec, rot_facteur);
        hierarchy_bird["body"].transform.rotate = tmpp;
    }
    if(user.keyboard_state.left){
        b = 1;
        rot_vec = {0,0,1};
    	tmpp = hierarchy_bird["body"].transform.rotate;
    	tmpp = tmpp * rotation(rot_vec, rot_facteur);
        hierarchy_bird["body"].transform.rotate = tmpp;
    }
    if(! b) {
        hierarchy_bird["body"].transform.translate += tmp * vec3(0,0,0.02f*(std::sin(2*3.14f*t)));
    }
	// Rotation of the shoulder-left around the y axis
    hierarchy_bird["shoulder_left"].transform.rotate   = rotation({0,0,1}, std::sin(2*3.14f*(t-0.4f)) );
	// Rotation of the arm-left around the y axis (delayed with respect to the shoulder)
    hierarchy_bird["arm_bottom_left"].transform.rotate = rotation({0,0,1}, std::sin(2*3.14f*(t-0.6f)) );
		
	// Rotation of the shoulder-right around the y axis
    hierarchy_bird["shoulder_right"].transform.rotate = rotation({0,0,-1}, std::sin(2*3.14f*(t-0.4f)) );
    // Rotation of the arm-right around the y axis (delayed with respect to the shoulder)
    hierarchy_bird["arm_bottom_right"].transform.rotate = rotation({0,0,-1}, std::sin(2*3.14f*(t-0.6f)) );
	
	// update the global coordinates
	hierarchy_bird.update_local_to_global_coordinates();
	// display the hierarchy_bird
	if(user.gui.display_surface)
		draw(hierarchy_bird, scene);
	if(user.gui.display_wireframe)
		draw_wireframe(hierarchy_bird, scene);
}

vec3 get_pos_bird(){
    vec3 pos = pos_without_oscill;
    return pos;
}
