extends Node2D

var levelIndex = 0
var levels = ["res://demo/levels/level_1_alpha.tscn", "res://demo/levels/level_2_alpha.tscn"]
var currentLevel = null

func _ready():
	loadLevel()

func getLevel():
	return levels[levelIndex]
	
func _process(delta: float) -> void:
	if TransitionHandler.transition:
		TransitionHandler.transition = false
		remove_child(currentLevel)
		nextLevel()
		loadLevel()
		
func nextLevel():
	levelIndex += 1
	
func loadLevel():
	var my_scene = load(getLevel()).instantiate()
	add_child(my_scene)
	currentLevel = my_scene
