extends CharacterBody2D

@export var speed: float = 300.0
@export var interaction_rotation_speed: float = 0.02
@export var interaction_distance: float = 10
@export var interaction_area_color: Color = Color(1.0, 1.0, 1.0, 1.0)
@export var interaction_hint_color: Color = Color(1.0, 1.0, 1.0, 1.0)
@export var interaction_hint_radius: float = 5
var draw_interaction_hint: bool = false
var draw_interaction_radius: bool = false
var interaction_hint_position = null
var lifting_object = false
var lifted_object:LightActor2D = null

func set_draw_interaction_hint(position) -> void:
	interaction_hint_position = position;
	draw_interaction_hint = true;
	queue_redraw()
	
func set_draw_interaction_radius() -> void:
	draw_interaction_radius = true;
	queue_redraw()

func _physics_process(delta):
	var direction = Input.get_vector("left", "right", "up", "down")
	if(direction):
		velocity = direction * speed
	else:
		velocity.x = move_toward(velocity.x, 0, speed)
		velocity.y = move_toward(velocity.y, 0, speed)
	move_and_slide()

func _process(delta: float) -> void:
	var player_position = get_global_transform().get_origin();
	var world = get_parent();
	var descendants = Descendants.get_descendants(world);
	var light_actors = []
	for descendant in descendants:
		if descendant is LightActor2D:
			light_actors.append(descendant as LightActor2D)
	
	var close_rotatable_actors = []
	var close_liftable_actors = []
	for untyped_light_actor in light_actors:
		var light_actor = untyped_light_actor as LightActor2D;
		var light_actor_position = light_actor.get_global_transform().get_origin()
		if light_actor.visible && light_actor.player_rotatable && player_position.distance_to(light_actor_position) < interaction_distance:
			close_rotatable_actors.append(light_actor)
		if light_actor.visible && light_actor.player_liftable && player_position.distance_to(light_actor_position) < interaction_distance:
			close_liftable_actors.append(light_actor)

	#find closest light actor that can be rotated (and is visible)
	var closest_rotatable_actor: LightActor2D = null;
	for close_rotatable_actor in close_rotatable_actors:
		if closest_rotatable_actor == null:
			closest_rotatable_actor = close_rotatable_actor
		else:
			var close_position = closest_rotatable_actor.get_global_transform().get_origin()
			var closest_position = closest_rotatable_actor.get_global_transform().get_origin()
			if player_position.distance_to(close_position) < player_position.distance_to(closest_position):
				closest_rotatable_actor = close_rotatable_actor
	#find closest light actor that can be lifted (and is visible)
	var closest_liftable_actor: LightActor2D = null;
	for close_liftable_actor in close_liftable_actors:
		if closest_liftable_actor == null:
			closest_liftable_actor = close_liftable_actor
		else:
			var close_position = close_liftable_actor.get_global_transform().get_origin()
			var closest_position = closest_liftable_actor.get_global_transform().get_origin()
			if player_position.distance_to(close_position) < player_position.distance_to(closest_position):
				closest_liftable_actor = close_liftable_actor

	var light_actor_rs = 0
	var light_actor_v = 0
	if Input.is_action_pressed("action_left"):
		set_draw_interaction_radius()
		light_actor_rs += interaction_rotation_speed
	if Input.is_action_pressed("action_right"):
		set_draw_interaction_radius()
		light_actor_rs += -interaction_rotation_speed
	if Input.is_action_just_pressed("lift_object"):
		set_draw_interaction_radius()
		if lifting_object:
			lifted_object.position = position
			lifted_object.visible = true
			lifted_object = null
			lifting_object = false;

		elif closest_liftable_actor != null && !lifting_object:
			lifted_object = closest_liftable_actor
			lifting_object = true
			lifted_object.visible = false
	if lifting_object:
		$CrateSprite.visible = true
	else:
		$CrateSprite.visible = false
		
	if closest_rotatable_actor != null:
		set_draw_interaction_hint(closest_rotatable_actor.get_global_transform().get_origin())
		closest_rotatable_actor.rotation += light_actor_rs
	queue_redraw()
	#drag liftable actor with you
	if closest_liftable_actor != null:
		set_draw_interaction_hint(closest_liftable_actor.get_global_transform().get_origin())
		closest_liftable_actor
	queue_redraw()
	
func _draw() -> void:
	if draw_interaction_radius:
		var point = Vector2(0.0, 0.0)
		var lower_alpha_color = interaction_area_color
		lower_alpha_color.a = 0.6
		draw_circle(point, interaction_distance, lower_alpha_color, true)
		draw_circle(point, interaction_distance, interaction_area_color, false, 3.0)
		draw_interaction_radius = false;
	if draw_interaction_hint:
		var point = interaction_hint_position - get_global_transform().get_origin(); 
		var lower_alpha_color = interaction_area_color
		lower_alpha_color.a = 0.6
		draw_circle(point, interaction_hint_radius, lower_alpha_color, true)
		draw_circle(point, interaction_hint_radius, interaction_area_color, false, 3.0)
		draw_interaction_hint = false;
