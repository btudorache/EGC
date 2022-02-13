#version 330

in vec3 world_position;

// Uniform properties
uniform sampler2D ballTexture;
uniform vec3 disco_ball_position;
uniform float enlapsed_time;

uniform int floor_lights_activated;
uniform int disco_light_activated;
uniform int reflector_lights_activated;

// Output
layout(location = 0) out vec4 out_color;

vec3 DiscoLightContribution();

void main()
{  
	out_color = vec4(DiscoLightContribution(), 1);
}

vec3 DiscoLightContribution()
{   
    if (disco_light_activated == 0) {
        return vec3(0, 0, 0);
    }

    vec3 light_dir = world_position - disco_ball_position;


    vec2 texcoord;
    texcoord.x = (1.0 / (2 * 3.14159)) * atan (light_dir.x, light_dir.z) + enlapsed_time;
    texcoord.y = (1.0 / 3.14159) * acos (light_dir.y / length (light_dir));
 
    vec3 color = texture(ballTexture, texcoord).xyz;

    return color;
}

