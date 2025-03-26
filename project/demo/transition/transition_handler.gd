extends Node

var transition_next_level_flag = false

func goto_next_level() -> void:
	transition_next_level_flag = true

func transition_complete() -> void:
	transition_next_level_flag = false
	
func transition_next_level() -> bool:
	return transition_next_level_flag
