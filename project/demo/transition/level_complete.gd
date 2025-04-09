extends CanvasLayer

var transition_speed = 0.3
func _ready() -> void:
	$NextLevel.pressed.connect(on_next_level_press);
		
func on_next_level_press():
	$AnimationPlayer.play("Shrink")
	get_parent().transition_out()
	#theres no built in wait function bruh
	await get_tree().create_timer(0.3).timeout
	TransitionHandler.goto_next_level()
