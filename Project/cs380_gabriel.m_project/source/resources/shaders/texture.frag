#version 440
in vec2 var_uv;
out vec4 out_color;
uniform sampler2D uniform_texture;
uniform int dim;
void main()
{
	vec3 texel = texture2D(uniform_texture, var_uv).rgb;
	if(dim == 1)
		out_color = vec4(vec3(texel.r), 1.0f);
	else if(dim == 3)
		out_color = vec4(texel, 1.0f);
}