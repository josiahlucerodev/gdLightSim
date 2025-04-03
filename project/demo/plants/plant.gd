class_name Plant

extends LightSensor2D

enum Type {FLOWER, MUSHROOM}
@export var type: Type = Type.FLOWER
@export var is_complete = false
@export var was_hit = false
