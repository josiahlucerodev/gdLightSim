extends Node2D

func _ready() -> void:
		Engine.max_fps = 60
		
func _process(delta: float) -> void:
	if $FPSCount.visible:
		$FPSCount.text = str(Engine.get_frames_per_second())

func transition_out() -> void:
	$AnimationPlayer.play("LevelFadeOut")
