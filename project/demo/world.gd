extends Node2D

func _process(delta: float) -> void:
	var completed = true
	var descendents = Descendants.get_descendants(self)
	for node in descendents:
		if node is Plant:
			var sensor_node = node as Plant
			if sensor_node.type == Plant.Type.MUSHROOM:
				completed = completed && !sensor_node.is_hit
			else:
				completed = completed && sensor_node.is_hit
	if completed:
		var level_complete = $"Level Complete"
		level_complete.visible = true
