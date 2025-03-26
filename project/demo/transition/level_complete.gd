extends CanvasLayer

func _ready() -> void:
	$NextLevel.pressed.connect(on_next_level_press);
		
func on_next_level_press():
	TransitionHandler.goto_next_level()
