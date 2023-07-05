#version 150

in  vec3 skybox_Position;
out vec4 skybox_Color;
uniform samplerCube skybox_Texture;

void main(){
  skybox_Color = texture(skybox_Texture,skybox_Position);
}