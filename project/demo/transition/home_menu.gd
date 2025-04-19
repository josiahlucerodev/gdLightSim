extends Node2D

@onready var exitButton = $HomeMenu/ExitButton
@onready var levelSelectButton = $HomeMenu/LevelSelectButton

func _ready() -> void:
	$HomeMenu.visible = true
	$LevelSelect.visible = false

func _process(delta: float) -> void:
	if exitButton.button_pressed:
		get_tree().quit()
	if levelSelectButton.button_pressed:
		$HomeMenu.visible = false
		$LevelSelect.visible = true
	if TransitionHandler.transition_home():
		$HomeMenu.visible = true
		$LevelSelect.visible = false
		TransitionHandler.transition_complete()
