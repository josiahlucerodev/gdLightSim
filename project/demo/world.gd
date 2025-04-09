extends Node2D
var completed = false
var animation_played = false

func _process(delta: float) -> void:
	if completed && !animation_played:
		var level_complete = $LightEnvironment2D/Border/LevelComplete
		level_complete.scale.x = 0
		level_complete.scale.y = 0
		$LightEnvironment2D/Border/LevelComplete/AnimationPlayer.play("Grow")
		level_complete.visible = true
		animation_played = true
	else:
		completed = true
		var descendents = Descendants.get_descendants(self)
		for node in descendents:
			if node is Plant:
				var sensor_node = node as Plant
				completed = completed && sensor_node.is_complete
	
