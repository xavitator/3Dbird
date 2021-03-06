#include "controller.hpp"

/**
 * @brief Appliquer la fonction opengl_uniform avec différents shader
 * 
 * @param shader shader à appliquer
 * @param current_scene scène de destination
 */
void opengl_uniform(GLuint shader, scene_environment const& current_scene)
{
	opengl_uniform(shader, "view", current_scene.camera.matrix_view());
	opengl_uniform(shader, "light", current_scene.light.matrix_view());
	opengl_uniform(shader, "projection", current_scene.projection);
}

/**
 * @brief Initialiser les données au lancement de l'application
 * 
 */
void initialize_data()
{
	//srand (time(NULL));
	GLuint const shader_mesh = opengl_create_shader_program(read_text_file("shader/mesh_with_shadow.vert.glsl"), read_text_file("shader/mesh_with_shadow.frag.glsl"));
	GLuint const shader_uniform_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	GLuint const texture_white = opengl_texture_to_gpu(image_raw{1,1,image_color_type::rgba,{255,255,255,255}});
	mesh_drawable::default_shader = shader_mesh;
	mesh_drawable::default_texture = texture_white;
	curve_drawable::default_shader = shader_uniform_color;
	segments_drawable::default_shader = shader_uniform_color;
	
	// Initialize the light position and viewpoint
	scene.light.distance_to_center = 20.0f;
	scene.light.manipulator_rotate_spherical_coordinates(pi/4.0f, pi/4.0f);

	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.gui.display_frame = false;
	init_camera();
	// Initialize the FBO and texture used to handle the depth map
	scene.depth_map = initialize_depth_map();
    // Create visual terrain surface
	tree = mesh_drawable(create_tree());
	
	caillou = mesh_drawable(mesh_load_file_obj("assets/caillou.obj"));
	caillou.shading.color={0.63f,0.63f,0.63f};
	caillou.transform.scale = 100;
	caillou.transform.rotate = rotation({ 1,0,0 }, 0.5f * pi);

	cloud1 = mesh_drawable(mesh_load_file_obj("assets/Cloud_1.obj"));
	cloud2 = mesh_drawable(mesh_load_file_obj("assets/Cloud_2.obj"));
	cloud3 = mesh_drawable(mesh_load_file_obj("assets/Cloud_3.obj"));
	cloud4 = mesh_drawable(mesh_load_file_obj("assets/Cloud_4.obj"));

	wall = mesh_drawable(create_wall());
	image_raw const im3 = image_load_png("assets/rayure1.png");
	// Send this image to the GPU, and get its identifier texture_image_id
	GLuint const texture_image_id3 = opengl_texture_to_gpu(im3,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);
	wall.texture = texture_image_id3;
	wall.shading.alpha = 0.5f;
    
	std::cout << "Le modele de bateau que nous utilisons entraine des warnings d'arete de taille 0 :" << std::endl;
	ship = mesh_drawable(mesh_load_file_obj("assets/ship.obj"));
	ship.transform.scale = 0.023f;
	ship.shading.color = { 0.24f,0.20f,0.2f };
	
	ring = mesh_drawable(mesh_load_file_obj("assets/cercle.obj"));
	ring.shading.color = { 1.0f,0,0 };
	ring.transform.scale = 0.5f;

	image_raw const im4 = image_load_png("assets/ocean.png");
	GLuint const texture_image_id4 = opengl_texture_to_gpu(im4,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);

	ocean_inf = mesh_drawable(create_ocean_infini());
	ocean_inf.texture = texture_image_id4;

	create_bird();

	restart_game();
}

/**
 * @brief Faire des actions à des intervalles de temps définis
 * 
 */
void update_from_timer(){
	period_1.update();
	period_10.update();
	if(period_1.event){
		user.score += 1;
	}
	if(period_10.event){
		vec3 tmp;
		bool b;
		Ring::get_position(ring_objects, tmp, b);
		if(b){
			Ring::add_ring(ring_objects, tmp);
		}
	}
}

/**
 * @brief Redémarrer le jeu après la mort de l'utilisateur avec de nouveaux paramètres
 * 
 */
void restart_game(){
	generate_terrain();
	user.dead = false;
	user.score = 0;
	user.speed = 0.001f;
	pos_without_oscill = { 0,0,5 };
	hierarchy_bird["body"].transform.translate = pos_without_oscill;
	orientation_bird = { 0,1,0 };
	hierarchy_bird["body"].transform.rotate = rotation();
	rho_theta_phi = { 5.0, 1.0, 1.0 };
	omega = 0.0f;
	theta = 0.0f;
	vitesse = 0.0f;
	timer = timer_basic();
	int choc;
	while ((choc = hit_ois()) >= 0 && choc <= 6)
		generate_terrain();
}

/**
 * @brief Typedef utilisé dans display_scene pour appeler 2 fois le meme bloc mais avec une fonction d'affichage différente (calcul d'ombre)
 * 
 */
using draw_func = std::function<void(mesh_drawable const& drawable, scene_environment const& current_scene)>;

/**
 * @brief Affichage d'une scène
 * 
 */
void display_scene()
{	
	update_ocean(ocean_m, ocean, parameters);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	std::function<void(draw_func)> draw_all = [](draw_func draw_element) -> void {
		ocean.transform.translate = { 0,0,0 };
		draw_element(ocean, scene);	
		for (int k = 0; k < liste_iles.size(); k++) {
			liste_iles[k].transform.translate = ile_position[k];
			//liste_iles[k].transform.rotate = rotation({ 0,0,1 }, ile_orientation[k]);
			draw_element(liste_iles[k], scene);
			int j = 0;
			for (int i = 0; i < liste_tree_position[k].size(); i++) {
				if (j < 2) {
					tree.transform.translate = liste_tree_position[k][i];
					tree.transform.translate += ile_position[k];
					draw_element(tree, scene);
					//ocean.transform.translate = { 0,0,10*i };
					//draw_element(ocean, scene);
					j++;
				}
				else
				{
					j = 0;
					caillou.transform.translate = liste_tree_position[k][i];
					caillou.transform.translate += ile_position[k] - vec3({0, 0, 0.1f});
					draw_element(caillou, scene);
				}
			}
		}
		for (int k = 0; k< ship_position.size(); k++) {
			ship.transform.translate = ship_position[k];
			ship.transform.translate += {0, 0, ocean_height(ship_position[k][0]+taille_terrain/2, ship_position[k][1] + taille_terrain / 2, taille_terrain, parameters) *0.8f};
			ship.transform.rotate = rotation({ { cos(ship_orientation[k]),0,sin(ship_orientation[k]) }, { sin(ship_orientation[k]),0,-cos(ship_orientation[k]) },{0,1,0} });
			//ship.transform.rotate = rotation({ 1,0,0 }, 0.5f * M_PI);
			draw_element(ship, scene);
		}
		int j = 3;
		for (int i = 0; i < cloud_position.size(); i++) {
			cloud_position[i] = cloud_deplacement(cloud_position[i]);
			if (j == 0) {
				
				cloud1.transform.translate = cloud_position[i];
				cloud1.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
				draw_element(cloud1, scene);
				j++;
			}
			else if (j == 1) {
				cloud2.transform.translate = cloud_position[i];
				cloud2.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
				draw_element(cloud2, scene);
				j++;
			}
			else if (j == 2) {
				cloud3.transform.translate = cloud_position[i];
				cloud3.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
				draw_element(cloud3, scene);
				j++;
			}
			else if (j == 3) {
				cloud4.transform.translate = cloud_position[i];
				cloud4.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
				draw_element(cloud4, scene);
				//j=0;
			}
		}
		for (int i = 0; i < ring_objects.size(); i++) {
			ring.transform.translate = ring_objects[i]->position;
			ring.shading.color = ring_objects[i]->color;
			ring.transform.rotate = rotation({ 0,0,1 }, ring_objects[i]->orientation);
			draw_element(ring, scene);
		}
		
		
	};

	// First pass: draw all shapes that cast shadows
	{
		glViewport(0, 0, scene.depth_map.width, scene.depth_map.height);
		glBindFramebuffer(GL_FRAMEBUFFER, scene.depth_map.fbo); opengl_check;
		glClear(GL_DEPTH_BUFFER_BIT);

		draw_all(draw_depth_map);

		glBindFramebuffer(GL_FRAMEBUFFER, 0); opengl_check;
		
	}

	// Second pass: Draw all shapes that receives shadows
	{
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw_all(draw_with_shadow);

	}
	for (int i = 0; i < ile_position_inf.size(); i++) {
		liste_iles[0].transform.translate = ile_position_inf[i];
		draw(liste_iles[0], scene);
	}
	ocean_inf.transform.translate = { 0,0,0 };
	draw(ocean_inf, scene);
	glDepthMask(false);
	wall.transform.rotate = rotation({ 0,0,1 }, 0);
	wall.transform.translate = { 0,0,0 };
	draw(wall, scene);

	wall.transform.translate = { 0.0f,-taille_terrain,0 };
	draw(wall, scene);

	wall.transform.rotate = rotation({ 0,0,1 }, 0.5f * pi);
	wall.transform.translate = { 0.0f,0,0 };
	draw(wall, scene);
	wall.transform.translate = { taille_terrain,0,0 };
	draw(wall, scene);

	glDepthMask(true);

	hierarchy_bird["body"].element.shading.color = color_bird;
	// std::for_each(hierarchy_bird.elements.begin(), hierarchy_bird.elements.end(), [](hierarchy_mesh_drawable_node el) {el.element.shading.color = color_bird;}); 

	if(! user.dead){
		move_bird();
		move_camera_center();
	}
	else {
		fall();
		draw(hierarchy_bird, scene);
		float rho = rho_theta_phi[0];
		float theta1 = pi / 4;
		float phi = pi / 4;
		vec3 pos_bird = get_pos_bird();
		vec3 pos_centered = { rho * std::sin(theta1) * std::cos(phi), rho * std::sin(theta1) * std::sin(phi), rho * std::cos(theta1) };
		scene.camera.look_at(pos_centered + pos_bird, pos_bird, { 0,0,rho });
		move_camera_center();
	}
}

/**
 * @brief Variable temporaire mais devant être globale pour changer la taille du terrain.
 * 
 */
int n_taille_terrain = taille_terrain;

/**
 * @brief Affichage de l'interface d'interaction avec l'utilisateur, notamment pour changer ses paramètres.
 * 
 */
void display_interface()
{	
	ImGui::Text("Score : %ld", user.score);
	ImGui::Separator();

	if(ImGui::CollapsingHeader("Aide")){
		ImGui::Text("L'objectif du jeu est de rester en vie autant que vous le pouvez. Vos points sont accumulés avec le temps.\n");
		ImGui::Text("Appuyer sur CTRL pour faire apparaître le curseur de la souris.\n");
		ImGui::Text("L'oiseau avance de plus en plus rapidement avec le temps.\n");
		ImGui::Text("Pour bouger l'oiseau, vous pouvez utiliser les commandes suivantes :\n");
		ImGui::BulletText(" AWSD\n");
		ImGui::BulletText(" QZSD\n");
		ImGui::BulletText(" les flèches du clavier\n");
		ImGui::Text("Voici les pouvoirs de chacun des cercles :\n");
		ImGui::BulletText(" cercle rouge : rapporte 10 points\n");
		ImGui::BulletText(" cercle bleu : rapporte 10 points mais augmente le facteur de vitesse\n");
		ImGui::BulletText(" cercle vert : rapporte 5 points et diminue le facteur de vitesse\n");
		ImGui::BulletText(" cercle or : rapporte 100 points\n");
		ImGui::BulletText(" cercle noir : il vous tue instantanément\n");
		ImGui::Text("Entre deux parties, vous pourrez changer les paramètres de génération de la carte.\n");
	}

	if (ImGui::CollapsingHeader("Paramètres"))
    {
		ImGui::Text("Vous pouvez modifier de jeu :");
        ImGui::SliderFloat("Sensibilité caméra", &fact_rot_cam, 0.001f, 0.01f);
        ImGui::SliderFloat("Sensibilité oiseau", &rot_facteur_bird, 0.01f, 0.1f);
        if(ImGui::ColorEdit3("Couleur de l'oiseau", color_bird.begin())){
			std::cout << color_bird << std::endl;
		}
    }
	if(user.dead && ImGui::CollapsingHeader("Jeu")){
		ImGui::Text("Vous pouvez modifier les paramètres d'initialisation :");
        ImGui::SliderInt("Terrain", &n_taille_terrain, 20, 150);
        ImGui::SliderInt("Îles", &nb_iles, 0, 100);
        ImGui::SliderInt("Arbres", &nb_arbres, 0, 100);
        ImGui::SliderInt("Nuages", &nb_cloud, 0, 100);
        ImGui::SliderInt("Bâteaux", &nb_ship, 0, 100);
        ImGui::SliderInt("Anneaux", &nb_ring, 0, 300);	
        ImGui::SliderFloat("Vitesse initiale", &initial_speed, 1.01f, 1.2f);
        ImGui::SliderInt("Hauteur de plafond", &ceiling_height, 5, taille_terrain);
	}
	if(user.dead){
		ImGui::Separator();
		if(ImGui::SmallButton("Genérer")){
			taille_terrain = n_taille_terrain;
			restart_game();
				
		}
	}
}