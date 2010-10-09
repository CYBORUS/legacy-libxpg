#version 150
// in_Position was bound to attribute index 0 and in_Color was bound to attribute index 1
uniform mat4 MVPM;
uniform mat4 MVM;
uniform mat4 NM;
uniform vec3 lightPosition;

in  vec4 in_Position;
in  vec4 in_Color;
in  vec3 in_Normal;

out vec4 ex_Color;
out vec3 ex_Normal;
out vec3 ex_LightDir;
void main(void)
{
    vec4 p = in_Position;
    p.w = 1.0;
    
    ex_Normal = vec3(NM * vec4(in_Normal, 1.0));
    
    vec4 vPosition4 = MVM * p;
    vec3 vPosition3 = vPosition4.xyz / vPosition4.w;
    
    ex_LightDir = normalize(lightPosition - vPosition3);
    
    gl_Position = MVPM * p;
    ex_Color = in_Color;
}

