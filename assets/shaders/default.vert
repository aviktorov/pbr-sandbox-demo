#version 450
#pragma shader_stage(vertex)

// Bindings
layout(push_constant) uniform Camera
{
	mat4 projection;
	mat4 view;
} camera;

// Input
layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec4 inColor;

// Output
layout(location = 0) out vec4 outColor;

void main() {
	gl_Position = camera.projection * camera.view * vec4(inPosition.xy, 0.0f, 1.0f);
	outColor = inColor;
}
