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
	
	var close_light_actors = []
	for untyped_light_actor in light_actors:
		var light_actor = untyped_light_actor as LightActor2D;
		var light_actor_position = light_actor.get_global_transform().get_origin()
		if light_actor.player_rotatable && player_position.distance_to(light_actor_position) < interaction_distance:
			close_light_actors.append(light_actor)
	
	var closest_light_actor: LightActor2D = null;
	for close_light_actor in close_light_actors:
		if closest_light_actor == null:
			closest_light_actor = close_light_actor
		else:
			var close_position = closest_light_actor.get_global_transform().get_origin()
			var closest_position = closest_light_actor.get_global_transform().get_origin()
			if player_position.distance_to(close_position) < player_position.distance_to(closest_position):
				closest_light_actor = close_light_actor

	var light_actor_rs = 0
	if Input.is_action_pressed("action_left"):
		set_draw_interaction_radius()
		light_actor_rs = interaction_rotation_speed
	if Input.is_action_pressed("action_right"):
		set_draw_interaction_radius()
		light_actor_rs = -interaction_rotation_speed
		
	if closest_light_actor != null:
		set_draw_interaction_hint(closest_light_actor.get_global_transform().get_origin())
		closest_light_actor.rotation += light_actor_rs
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
