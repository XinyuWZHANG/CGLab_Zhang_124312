#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_position;

out vec3 skybox_Position;
uniform mat4 ProjectionMatrix;
uniform mat4 ViewMatrix;

void main(){
	skybox_Position = in_position;
	gl_Position = ProjectionMatrix * ViewMatrix * ve4(position,1.0);
}