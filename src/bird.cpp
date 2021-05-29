#include "vcl/vcl.hpp"
#include "variable.hpp"
#include <iostream>
#include <typeinfo>  //for 'typeid' to work  
#include <cmath>
#include "hitbox.hpp";
#include "terrain.hpp";


using namespace vcl;

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
    float const length_arm = 0.3f;
	float const radius_head = radius_body/2;
	float const radius_noise = radius_head/2;
    float const length_tail = 0.4f;

    // The geometry of the body is a sphere
    mesh_drawable body = mesh_drawable( mesh_primitive_ellipsoid({radius_body*2/3, radius_body*2, radius_body*2/3}, {0,0,0}, 40, 40));

    mesh_drawable head = mesh_drawable( mesh_primitive_ellipsoid({radius_head, radius_head*1.1f, radius_head}, {0,0,0}, 40, 40));

    //mesh_drawable noise = mesh_drawable( mesh_primitive_triangle({radius_noise, radius_noise*2, 0}, {0,0,0}, 40, 40));
    mesh_drawable noise = mesh_drawable( create_cone_bird(radius_noise/2, 3*radius_noise, 0) );
	noise.shading.color = {0,0,0};
	noise.transform.rotate =  rotation({1,0,0}, -3.14f/2 );

	// Geometry of the eyes: black spheres
	mesh_drawable eye = mesh_drawable(mesh_primitive_sphere(0.03f, {0,0,0}, 20, 20));
    eye.shading.color = {0,0,0};

	// Shoulder part and arm are displayed as cylinder
    mesh_drawable shoulder_left = mesh_drawable(mesh_primitive_quadrangle({0,0,0}, {0,radius_body,0}, {-length_arm, radius_body * 1.5f,0}, {-length_arm, radius_body/2,0}));
    mesh_drawable arm_left = mesh_drawable(mesh_primitive_triangle({0, radius_body * 1.5f,0}, {0, radius_body/2,0}, {-length_arm,0, 0}));

	mesh_drawable shoulder_right = mesh_drawable(mesh_primitive_quadrangle({0,0,0}, {0,radius_body,0}, {length_arm, radius_body * 1.5f,0}, {length_arm, radius_body/2,0}));
    mesh_drawable arm_right = mesh_drawable(mesh_primitive_triangle({0, radius_body * 1.5f,0}, {0, radius_body/2,0}, {length_arm,0, 0}));
    
    // queue de l'hirondelle
    mesh_drawable tail_left = mesh_drawable(mesh_primitive_triangle({radius_body/4, 0,0}, {-radius_body/4, 0,0}, {-radius_body/3.0f,-length_tail, 0}));
    mesh_drawable tail_right = mesh_drawable(mesh_primitive_triangle({-radius_body/4, 0,0}, {radius_body/4, 0,0}, {radius_body/3.0f,-length_tail, 0}));

	// An elbow displayed as a sphere
    // mesh_drawable elbow = mesh_drawable(mesh_primitive_sphere(0.055f));


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
    hierarchy_bird.add(shoulder_left, "shoulder_left", "body", {-radius_body/2+0.05f,0,0}); // extremity of the spherical body
    // hierarchy_bird.add(elbow, "elbow_left", "shoulder_left", {-length_arm,0,0});          // place the elbow the extremity of the "shoulder cylinder"
    hierarchy_bird.add(arm_left, "arm_left", "shoulder_left", {-length_arm,0,0});                        // the arm start at the center of the elbow

	// Set the right part of the body arm: similar to the left part with a symmetry in x direction
    hierarchy_bird.add(shoulder_right, "shoulder_right", "body", {radius_body/2-0.05f,0,0});
    // hierarchy_bird.add(elbow, "elbow_right", "shoulder_right", {length_arm,0,0});
    hierarchy_bird.add(arm_right, "arm_right", "shoulder_right", {length_arm,0,0});

    hierarchy_bird.add(tail_left, "tail_left", "body", {0, -radius_body*2.0f + 0.05f, 0});
    hierarchy_bird.add(tail_right, "tail_right", "tail_left");


    hierarchy_bird["body"].transform.translate = { 0,0,5 };
    pos_without_oscill = hierarchy_bird["body"].transform.translate;
}

void come_back_to_earth(){
    mat3 tmp = hierarchy_bird["body"].transform.rotate.matrix();

    vec3 direction = tmp * orientation_bird;

    vec3 perpend =  tmp * orthogonal_vector(orientation_bird);

    if(direction[2] > 0){
        rotation tmp_rot = hierarchy_bird["body"].transform.rotate;
        hierarchy_bird["body"].transform.rotate = tmp_rot * rotation(perpend, 2 * rot_facteur_bird);
    }
}

void move_bird(){
    /** *************************************************************  **/
    /** Compute the (animated) transformations applied to the elements **/
    /** *************************************************************  **/

    timer.update();
    float t = timer.t;


    // The body oscillate along the z direction
    hierarchy_mesh_drawable_node bird = hierarchy_bird["body"];

    vec3 rot_vec;
    rotation tmpp;
    int b = 0;
    if(user.keyboard_state.up){
        rot_vec = {1,0,0};
        b = 1;
        tmpp = hierarchy_bird["body"].transform.rotate;
    	tmpp = tmpp * rotation(rot_vec, rot_facteur_bird);
        hierarchy_bird["body"].transform.rotate = tmpp;
    }
    if(user.keyboard_state.down){
        b = 1;
        rot_vec = {-1,0,0};
        tmpp = hierarchy_bird["body"].transform.rotate;
    	tmpp = tmpp * rotation(rot_vec, rot_facteur_bird);
        hierarchy_bird["body"].transform.rotate = tmpp;
    }
    if(user.keyboard_state.right){
        b = 1;
        rot_vec = {0,0,-1};
    	tmpp = hierarchy_bird["body"].transform.rotate;
    	tmpp = tmpp * rotation(rot_vec, rot_facteur_bird);
        hierarchy_bird["body"].transform.rotate = tmpp;
    }
    if(user.keyboard_state.left){
        b = 1;
        rot_vec = {0,0,1};
    	tmpp = hierarchy_bird["body"].transform.rotate;
    	tmpp = tmpp * rotation(rot_vec, rot_facteur_bird);
        hierarchy_bird["body"].transform.rotate = tmpp;
    }


    if(pos_without_oscill[2] > ceiling_height){
        come_back_to_earth();
    }

    // apply next step for the bird
    mat3 tmp = hierarchy_bird["body"].transform.rotate.matrix();
    vec3 ajout_translat = std::log(user.speed*t + initial_speed) * tmp * orientation_bird;
    hierarchy_bird["body"].transform.translate += ajout_translat;
    pos_without_oscill += ajout_translat;

    if(! b) {
        hierarchy_bird["body"].transform.translate += tmp * vec3(0,0,0.02f*(std::sin(2*3.14f*t)));
    }




    if(! user.keyboard_state.left){
        // Rotation of the shoulder-left around the y axis
        hierarchy_bird["shoulder_left"].transform.rotate = rotation({0,1,0}, std::sin(2*3.14f*(t-0.4f)) );
        // Rotation of the arm-left around the y axis (delayed with respect to the shoulder)
        hierarchy_bird["arm_left"].transform.rotate = rotation({0,1,0}, std::sin(2*3.14f*(t-0.6f)) );
    }

    if(user.keyboard_state.right){
        // Rotation of the shoulder-right around the y axis
        hierarchy_bird["shoulder_right"].transform.rotate = rotation({0,1,0}, 45 * pi/180.0f );
        // Rotation of the arm-right around the y axis (delayed with respect to the shoulder)
        hierarchy_bird["arm_right"].transform.rotate = rotation({0,1,0}, 45 * pi/180.0f );
    }

    if(user.keyboard_state.left){
        // Rotation of the shoulder-left around the y axis
        hierarchy_bird["shoulder_left"].transform.rotate = rotation({0,-1,0}, 45 * pi/180.0f );
        // Rotation of the arm-left around the y axis (delayed with respect to the shoulder)
        hierarchy_bird["arm_left"].transform.rotate = rotation({0,-1,0}, 45 * pi/180.0f );
    }
		
    if(! user.keyboard_state.right){

        // Rotation of the shoulder-right around the y axis
        hierarchy_bird["shoulder_right"].transform.rotate = rotation({0,-1,0}, std::sin(2*3.14f*(t-0.4f)) );
        // Rotation of the arm-right around the y axis (delayed with respect to the shoulder)
        hierarchy_bird["arm_right"].transform.rotate = rotation({0,-1,0}, std::sin(2*3.14f*(t-0.6f)) );
    }



    if(user.keyboard_state.up){
        hierarchy_bird["tail_left"].transform.rotate = rotation({-1,0,0}, 45 * pi/180.0f);
    }
    else if(user.keyboard_state.down){
        hierarchy_bird["tail_left"].transform.rotate = rotation({1,0,0}, 45 * pi/180.0f);
    }
    else {
        hierarchy_bird["tail_left"].transform.rotate = rotation();
    }

	// update the global coordinates
	hierarchy_bird.update_local_to_global_coordinates();
	// display the hierarchy_bird
	draw(hierarchy_bird, scene);
	if(user.gui.display_wireframe)
		draw_wireframe(hierarchy_bird, scene);
}

vec3 get_pos_bird(){
    vec3 pos = pos_without_oscill;
    return pos;
}


void fall() {


    // Rotation of the shoulder-right around the y axis
    hierarchy_bird["shoulder_right"].transform.rotate = rotation({0,1,0}, 45 * pi/180.0f );
    // Rotation of the arm-right around the y axis (delayed with respect to the shoulder)
    hierarchy_bird["arm_right"].transform.rotate = rotation({0,1,0}, 45 * pi/180.0f );

    // Rotation of the shoulder-left around the y axis
    hierarchy_bird["shoulder_left"].transform.rotate = rotation({0,-1,0}, 45 * pi/180.0f );
    // Rotation of the arm-left around the y axis (delayed with respect to the shoulder)
    hierarchy_bird["arm_left"].transform.rotate = rotation({0,-1,0}, 45 * pi/180.0f );

    hierarchy_bird["tail_left"].transform.rotate = rotation();

    vec3 pos = get_pos_bird();
    int k = on_ile();
    if (k > -1 && hit_ile(k)>-1) return;
    float t = timer.t;
    timer.update();
    float dt = timer.t - t;
    std::cout << theta << std::endl;
    if (theta < pi / 2) {
        std::cout << "theta" << theta << std::endl;
        omega = omega + cos(theta) * dt * 5;
        theta = theta + omega * dt;
        hierarchy_bird["body"].transform.rotate = rotation({ 1,0,0 }, -theta);
        hierarchy_bird.update_local_to_global_coordinates();
    }
    else {
        hierarchy_bird["body"].transform.rotate = rotation({ 1,0,0 }, -pi / 2);
        hierarchy_bird.update_local_to_global_coordinates();
    }
    
    float z = ocean_height(pos[0] + taille_terrain / 2, pos[1] + taille_terrain / 2, taille_terrain, parameters);
    if (z > pos[2]-0.5f ) {
        hierarchy_bird["body"].transform.translate = { pos[0],pos[1],z };
        hierarchy_bird.update_local_to_global_coordinates();
        //pos_without_oscill = hierarchy_bird["body"].transform.translate;
        return;
    }
    
    float g = 10.0f;
    vitesse = vitesse + g * dt;
    hierarchy_bird["body"].transform.translate = hierarchy_bird["body"].transform.translate - vec3{0, 0, vitesse *dt};
    hierarchy_bird.update_local_to_global_coordinates();
    pos_without_oscill = hierarchy_bird["body"].transform.translate;
    return;
}