extends CanvasLayer

var transition_speed = 0.3
func _ready() -> void:
	if TransitionHandler.is_last_level():
		$Level.visible = false;
		$Game.visible = true;
		$Game/Exit.pressed.connect(on_exit_press);
	else:
		$Level.visible = true
		$Game.visible = false;
		$Level/NextLevel.pressed.connect(on_next_level_press);

func on_exit_press():
	$AnimationPlayer.play("Shrink")
	get_parent().transition_out()
	await get_tree().create_timer(0.3).timeout
	TransitionHandler.goto_level_select(true)
	
func on_next_level_press():
	$AnimationPlayer.play("Shrink")
	get_parent().transition_out()
	await get_tree().create_timer(0.3).timeout
	TransitionHandler.goto_next_level()
