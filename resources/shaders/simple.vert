#version 150
#extension GL_ARB_explicit_attrib_location : require
// vertex attributes of VAO
layout(location = 0) in vec3 in_Position;
layout(location = 1) in vec3 in_Normal;

//Assignment 4
layout(location = 2) in vec2 in_TexCoord;

//Matrix Uniforms as specified with glUniformMatrix4fv
uniform mat4 ModelMatrix;
uniform mat4 ViewMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 NormalMatrix;

out vec3 pass_Normal;
out vec4 passedPosition;

//Assignment 4
out vec2 pass_TexCoord;

void main(void)
{
	gl_Position = (ProjectionMatrix  * ViewMatrix * ModelMatrix) * vec4(in_Position, 1.0);
	//new position
	passedPosition = ModelMatrix * vec4(in_Position, 1.0f);
	//modifying normal vector in world space
	pass_Normal = normalize(vec3(passedPosition - ModelMatrix * vec4(0,0,0,1)));

	//Assignment 4
	pass_TexCoord = in_TexCoord;
}
