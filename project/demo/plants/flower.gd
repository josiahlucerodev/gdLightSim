extends Plant

func _process(delta: float) -> void:
	if was_hit != is_hit:
		$Timer.start()
	was_hit = is_hit
	if is_hit:
		$BloomedFlower.visible = true
		$UnbloomedFlower.visible = false
	else:
		$BloomedFlower.visible = false
		$UnbloomedFlower.visible = true


func _on_timer_timeout() -> void:
	is_complete = is_hit
