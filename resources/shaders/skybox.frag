#version 150

in  vec3 skybox_Position;
out vec4 skybox_Color;
uniform samplerCube skybox_texture;

void main(){
  skybox_Color = texture(skybox_texture,skybox_Position);
}