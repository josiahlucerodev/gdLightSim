extends Plant

func _ready() -> void:
	is_complete = !is_hit

func _process(delta: float) -> void:
	if is_hit:
		is_complete = false
	if was_hit != is_hit:
		$Timer.start()
	was_hit = is_hit
	if is_hit:
		$MushroomSprite.scale = Vector2(0.1 * 0.7,0.1)
	else:
		$MushroomSprite.scale = Vector2(0.1,0.1)
		
func _on_timer_timeout():
		is_complete = !is_hit
