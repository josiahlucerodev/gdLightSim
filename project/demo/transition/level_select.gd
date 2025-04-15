extends Node2D

var level_index = 0
var levels = [
"res://demo/levels/level_1_tutorial.tscn", 
"res://demo/levels/level_2_tutorial.tscn", 
"res://demo/levels/level_3_tutorial.tscn", 
"res://demo/levels/level_4_zigzag.tscn",
"res://demo/levels/level_5_beta.tscn",
"res://demo/levels/level_6_grid.tscn",
"res://demo/levels/level_7_mushring.tscn",
"res://demo/levels/level_8_angle.tscn"]

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
	
@onready var button_visable_array = [] 

func _ready():
	button_visable_array.resize(buttons.size())
	button_visable_array.fill(false);
	button_visable_array[0] = true;
		
	update_buttons();

func update_buttons():
	for i in range(buttons.size()):
		buttons[i].visible = button_visable_array[i];

func getLevel():
	return levels[level_index]
	
func _process(delta: float) -> void:
	if TransitionHandler.transition_next_level():
		TransitionHandler.transition_complete()
		nextLevel()
	
	if TransitionHandler.transition_level_select():
		remove_child(current_level)
		current_level = null
		if level_index != 0:
			button_visable_array[level_index + 1] = TransitionHandler.is_level_completed();
		update_buttons()
		button_panel.visible = true
		TransitionHandler.transition_complete()
		
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
	button_visable_array[level_index] = true;
	
	loadLevel()
	
func loadLevel():
	var my_scene = load(getLevel()).instantiate()
	add_child(my_scene)
	current_level = my_scene
	button_panel.visible = false;
