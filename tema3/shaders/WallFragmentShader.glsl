#version 330

// Input
in vec3 world_position;
in vec3 world_normal;

// Uniforms for light properties
// light_direction -> used for spotlight light
uniform vec3 light_direction;
uniform vec3 light_position;
uniform vec3 light_color;
uniform vec3 eye_position;

uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform float spot_light_angle;

uniform vec3 spot_light_position[4];
uniform vec3 spot_light_direction[4];
uniform vec3 spot_light_color[4];

uniform vec3 object_color;

uniform vec3 disco_ball_position;
uniform float enlapsed_time;
uniform sampler2D ballTexture;

uniform int floor_lights_activated;
uniform int disco_light_activated;
uniform int reflector_lights_activated;

// Output
layout(location = 0) out vec4 out_color;

vec3 PointLightContribution(vec3 lightPos, vec3 lightColor);

vec3 DiscoLightContribution();

vec3 ConeLightContribution();

void main()
{
    vec3 color = object_color * PointLightContribution(light_position, light_color) + 0.6 * DiscoLightContribution() + ConeLightContribution();

    out_color = vec4(color, 1);
}

vec3 PointLightContribution(vec3 lightPos, vec3 lightColor)
{
    if (floor_lights_activated == 0) {
        return vec3(0, 0, 0);    
    }

    vec3 L = normalize(lightPos - world_position);
    vec3 V = normalize(eye_position - world_position);
    vec3 H = normalize(L + V);

    float diffuse_light = material_kd * max(dot(world_normal, L), 0);
    float specular_light = 0;
    
    if (diffuse_light > 0) {
        vec3 R = reflect(-L, world_normal);
        specular_light = material_ks * pow(max(dot(V, R), 0.0), material_shininess);
    }

    float dist = distance(lightPos, world_position);
    float mitigationFactor = 1;
    float light_radius = 3;
    if (dist < light_radius) {
        mitigationFactor = pow(light_radius - dist, 2);
    }

	vec3 color = lightColor * 2 * mitigationFactor * (10 * diffuse_light + 0.2 * specular_light);
	
	return color;
}

vec3 DiscoLightContribution()
{
    if (disco_light_activated == 0) {
        return vec3(0, 0, 0);
    }

    vec3 L = normalize(disco_ball_position - world_position);
    if (max(dot(world_normal, L), 0) == 0) {
        return vec3(0, 0, 0);
    }

    // light_dir  este directia de iluminare
    vec3 light_dir = world_position - disco_ball_position;
 
    // texcoord este coordonata de textura utilizata pentru esantionare
    vec2 texcoord;
    texcoord.x = (1.0 / (2 * 3.14159)) * atan (light_dir.x, light_dir.z) + enlapsed_time;
    texcoord.y = (1.0 / 3.14159) * acos (light_dir.y / length (light_dir));
 
    // color este culoarea corespunzatoare pozitiei world_position
    vec3 color = texture(ballTexture, texcoord).xyz;

    return color;
}

vec3 ConeLightContribution() 
{
    if (reflector_lights_activated == 0) {
        return vec3(0);
    }

    vec3 color = vec3(0);
    for (int i = 0; i < 4; i++) {
        vec3 L = normalize(spot_light_position[i] - world_position);
        vec3 V = normalize(eye_position - world_position);
        vec3 H = normalize(L + V);

        float ambient_light = 0.5 * material_kd;
        float diffuse_light = material_kd * max(dot(world_normal, L), 0);
        float specular_light = 0;

        if (diffuse_light > 0) {
            vec3 R = reflect(-L, world_normal);
            specular_light = material_ks * pow(max(dot(V, R), 0.0), material_shininess);
        }


        float light_att_factor = 1;

        float cut_off = radians(spot_light_angle);
        float spot_light = dot(-L, spot_light_direction[i]);
        float spot_light_limit = cos(cut_off);
    
        if (spot_light > spot_light_limit) {
            // Quadratic attenuation
            float linear_att = (spot_light - spot_light_limit) / (1.0f - spot_light_limit);
            light_att_factor = pow(linear_att, 2);
        } else {
            diffuse_light = 0;
            specular_light = 0;
        }

        color += vec3(0.01, 0.01, 0.01) * spot_light_color[i] * (ambient_light + light_att_factor * (300 * diffuse_light + 300 * specular_light));
    }

    return color;
}
