extends Node2D

func _process(delta: float) -> void:
	var completed = true
	var descendents = Descendants.get_descendants(self)
	for node in descendents:
		if node is Plant:
			var sensor_node = node as Plant
			completed = completed && sensor_node.is_complete
	if completed:
		var level_complete = $"LevelComplete"
		level_complete.visible = true
