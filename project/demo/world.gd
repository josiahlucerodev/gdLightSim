extends Node2D

func _process(delta: float) -> void:
	var completed = true
	var descendents = get_all_descendants(self)
	for node in descendents:
		if node is SensorTypes:
			var sensor_node = node as SensorTypes
			if sensor_node.type == SensorTypes.Type.MUSHROOM:
				completed = completed && !sensor_node.is_hit
			else:
				completed = completed && sensor_node.is_hit
	if completed:
		var level_complete = $"Level Complete"
		level_complete.visible = true
		

func get_all_descendants(node):
	var all_descendants = []
	for child in node.get_children():
		all_descendants.append(child)
		all_descendants += get_all_descendants(child) 
	return all_descendants
		
	
