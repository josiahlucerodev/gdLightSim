shader_type canvas_item;

uniform sampler2D backgroundSampler;

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	vec4 texColor = texture(backgroundSampler, fract(SCREEN_UV * vec2(10, 10)));
	COLOR = texColor;
}

//void light() {
//	// Called for every pixel for every light affecting the CanvasItem.
//	// Uncomment to replace the default light processing function with this one.
//}
