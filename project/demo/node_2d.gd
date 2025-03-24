extends Node2D
var rotating = false
var ang_velocity = 0
func _process(delta: float) -> void:
	if Input.is_action_pressed("cw"):
		ang_velocity = 0.01
	if Input.is_action_pressed("ccw"):
		ang_velocity = -0.01
	if !Input.is_action_pressed("ccw") && !Input.is_action_pressed("cw"):
		ang_velocity = 0
	global_transform = global_transform.rotated(ang_velocity)
	
