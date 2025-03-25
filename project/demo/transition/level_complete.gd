extends CanvasLayer

func _ready() -> void:
	$nextLevel.pressed.connect(on_next_level_press);
		
func on_next_level_press():
	TransitionHandler.transition = true
