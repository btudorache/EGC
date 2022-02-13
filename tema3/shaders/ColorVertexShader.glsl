#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;
uniform vec3 Color;

// Output
out vec3 frag_color;

void main()
{
    frag_color = Color;

    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
