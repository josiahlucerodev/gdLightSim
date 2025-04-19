extends Node2D

var level_index = 0
var levels = [
"res://demo/levels/level_1_tutorial.tscn", 
"res://demo/levels/level_2_tutorial.tscn", 
"res://demo/levels/level_3_tutorial.tscn", 
"res://demo/levels/level_4_zigzag.tscn",
"res://demo/levels/level_5_mush_angle.tscn",
"res://demo/levels/level_6_beta.tscn",
"res://demo/levels/level_7_grid.tscn",
"res://demo/levels/level_8_angle.tscn",
"res://demo/levels/level_9_mushring.tscn"]

var current_level = null

@onready var button_panel = $BP

@onready var buttons = [ 
	$BP/R1/L1,
	$BP/R1/L2,
	$BP/R1/L3,
	$BP/R1/L4,
	$BP/R1/L5,
	$BP/R2/L6, 
	$BP/R2/L7, 
	$BP/R2/L8, 
	$BP/R2/L9, 
	$BP/R2/L10, 
	$BP/R3/L11,
	$BP/R3/L12,
	$BP/R3/L13,
	$BP/R3/L14,
	$BP/R3/L15]
	
@onready var button_visible_array = [] 
@onready var home_button = $BP/Home

func _ready():
	button_visible_array.resize(buttons.size())
	button_visible_array.fill(false);
	load_data()
	for i in range(level_index + 1):
			button_visible_array[i] = true
	update_buttons();
	
func save_file(data):
	var file = FileAccess.open("res://levelData.dat",FileAccess.WRITE)
	file.store_8(data)
	file.close()
	
func load_data():
	if !FileAccess.file_exists("res://levelData.dat"):
		save_file(level_index)
	else:
		var file = FileAccess.open("res://levelData.dat", FileAccess.READ)
		level_index = file.get_8()
		file.close()

func update_buttons():
	for i in range(buttons.size()):
		buttons[i].visible = button_visible_array[i];

func getLevel():
	return levels[level_index]
	
func _process(delta: float) -> void:
	if home_button.button_pressed:
		TransitionHandler.goto_home()
	
	if TransitionHandler.transition_next_level():
		TransitionHandler.transition_complete()
		nextLevel()
	
	if TransitionHandler.transition_level_select():
		remove_child(current_level)
		current_level = null
		if level_index != 0:
			button_visible_array[level_index + 1] = button_visible_array[level_index + 1] || TransitionHandler.is_level_completed();
		update_buttons()
		button_panel.visible = true
		TransitionHandler.selected_level()
		
	#Ctrl + Shift + L in level select shows all levels
	if(button_panel.visible && Input.is_action_just_pressed("debug_level_select")):
		for i in range(buttons.size()):
			button_visible_array[i] = true
			update_buttons()
		
	for i in range(buttons.size()):
		var button = buttons[i];
		if button.button_pressed:
			level_index = i;
			loadLevel()
			button.button_pressed = false
			pass
func nextLevel():
	remove_child(current_level)
	level_index += 1
	save_file(level_index)
	button_visible_array[level_index] = true;
	
	loadLevel()
	
func loadLevel():
	var my_scene = load(getLevel()).instantiate()
	add_child(my_scene)
	current_level = my_scene
	button_panel.visible = false;
