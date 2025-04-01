extends Plant

func _process(delta: float) -> void:
	if is_hit:
		$MushroomSprite.scale = Vector2(0.1 * 0.7,0.1)
	else:
		$MushroomSprite.scale = Vector2(0.1,0.1)
