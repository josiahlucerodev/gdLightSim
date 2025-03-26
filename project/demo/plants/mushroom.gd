extends Plant

func _process(delta: float) -> void:
	if is_hit:
		$MushroomSprite.scale = Vector2(0.145 * 0.7,0.145 * 0.7)
	else:
		$MushroomSprite.scale = Vector2(0.145,0.145)
