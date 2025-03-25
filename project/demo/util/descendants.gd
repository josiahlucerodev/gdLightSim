extends Node

func get_descendants(node):
	var descendants = []
	for child in node.get_children():
		descendants.append(child)
		descendants += get_descendants(child) 
	return descendants
