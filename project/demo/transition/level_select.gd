extends Node2D

var level_index = 0
var levels = ["res://demo/levels/level_8_angle.tscn",
"res://demo/levels/level_1_tutorial.tscn", 
"res://demo/levels/level_2_tutorial.tscn", 
"res://demo/levels/level_3_tutorial.tscn", 
"res://demo/levels/level_4_zigzag.tscn",
"res://demo/levels/level_5_beta.tscn",
"res://demo/levels/level_6_grid.tscn",
"res://demo/levels/level_7_mushring.tscn"]
var current_level = null

func _ready():
	loadLevel()

func getLevel():
	return levels[level_index]
	
func _process(delta: float) -> void:
	if TransitionHandler.transition_next_level():
		TransitionHandler.transition_complete()
		nextLevel()
		
func nextLevel():
	remove_child(current_level)
	level_index += 1
	loadLevel()
	
func loadLevel():
	var my_scene = load(getLevel()).instantiate()
	add_child(my_scene)
	current_level = my_scene
