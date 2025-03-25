extends Plant

func _process(delta: float) -> void:
	if is_hit:
		$BloomedFlower.visible = true
		$UnbloomedFlower.visible = false
	else:
		$BloomedFlower.visible = false
		$UnbloomedFlower.visible = true
