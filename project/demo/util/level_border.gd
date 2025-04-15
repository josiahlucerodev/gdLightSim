extends Node2D

func _ready() -> void:
		Engine.max_fps = 60
		
func _process(delta: float) -> void:
	if $FPSCount.visible:
		$FPSCount.text = str(Engine.get_frames_per_second())
	
	if $ExitButton.button_pressed:
		TransitionHandler.goto_level_select(false);

func transition_out() -> void:
	$AnimationPlayer.play("LevelFadeOut")
