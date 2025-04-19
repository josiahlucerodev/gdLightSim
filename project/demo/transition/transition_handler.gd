extends Node

var transition_next_level_flag = false
var transition_to_level_complete_flag = false;
var transition_level_completed = false;
var transition_back_to_home_flag = false;
var last_level_flag = false;

func transition_complete() -> void:
	transition_next_level_flag = false
	transition_to_level_complete_flag = false
	transition_level_completed = true;
	transition_back_to_home_flag = false;
	last_level_flag = false;
	
func selected_level() -> void:
	transition_next_level_flag = false
	transition_to_level_complete_flag = false

func goto_next_level() -> void:
	transition_next_level_flag = true

func transition_next_level() -> bool:
	return transition_next_level_flag
	
func goto_level_select(completed: bool) -> void:
	transition_to_level_complete_flag = true
	transition_level_completed = completed
	
func transition_level_select() -> bool:
	return transition_to_level_complete_flag

func is_level_completed() -> bool:
	return transition_level_completed

func goto_home() -> void:
	transition_back_to_home_flag = true;
	
func transition_home() -> bool:
	return transition_back_to_home_flag
	
func last_level() -> void:
	last_level_flag = true

func is_last_level() -> bool:
	return last_level_flag
