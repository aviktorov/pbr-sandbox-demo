#version 450
#pragma shader_stage(vertex)

// Input
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

// Output
layout(location = 0) out vec4 outColor;

void main() {
	gl_Position = vec4(inPosition.xy, 1.0f, 1.0f);
	outColor = inColor;
}
