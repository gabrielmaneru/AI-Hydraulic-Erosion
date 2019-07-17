#version 440
in vec2 uv;
uniform vec3 base_color;
out vec4 out_color;

uniform bool useColor = true;
uniform sampler2D reflection_texture;
uniform sampler2D refraction_texture;
uniform float reflect_factor;
uniform float color_factor;
uniform float win_width;
uniform float win_height;

void main()
{
	if(useColor)
	{
		out_color = vec4(0.12, 0.45, 0.5, 1.0);
		return;
	}
	
	float dx = gl_FragCoord.x/win_width;
	float dy = gl_FragCoord.y/win_height;

	vec4 reflection_color = texture(reflection_texture, vec2(dx,1-dy));
	vec4 refraction_color = texture(refraction_texture, vec2(dx, dy));
	
	out_color = color_factor * vec4(base_color, 1.0) + (1-color_factor) * (reflect_factor * reflection_color + (1.0f - reflect_factor) * refraction_color);
}