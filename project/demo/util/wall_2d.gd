class_name Wall2D

extends LightColider2D

@onready var static_body_2d = $'StaticBody2D'

func _ready():
	(static_body_2d.get_child(0) as CollisionPolygon2D).polygon = polygon
