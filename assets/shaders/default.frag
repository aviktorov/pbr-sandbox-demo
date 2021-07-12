#version 450
#pragma shader_stage(fragment)

// Input
layout(location = 0) in vec4 inColor;

// Output
layout(location = 0) out vec4 outFramebufferColor;

void main()
{
	outFramebufferColor = inColor;
}
