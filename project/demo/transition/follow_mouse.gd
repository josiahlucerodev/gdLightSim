extends Node2D

func _process(delta: float) -> void:
	var mouse_pos = get_global_mouse_position()
	var direction = mouse_pos - global_position
	rotation = direction.angle()
