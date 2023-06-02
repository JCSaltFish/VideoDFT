// From: https://www.shadertoy.com/view/XtScWt

#version 430

layout(location = 0) uniform sampler2D tex;

layout(location = 1) uniform int pass = 0;
layout(location = 2) uniform int size;
layout(location = 3) uniform int mode = 0;
const int MODE_DFT = 1;
const int MODE_IFT = 2;

in vec2 tex_coord;
out vec4 fragcolor;

vec2 cmul (vec2 a,float b) { return mat2(a,-a.y,a.x) * vec2(cos(b),sin(b)); }

float average(vec3 v)
{
	return dot(v, vec3(1.0)) / 3.0;   
}

void main()
{
	if (mode != 0)
	{
		if (gl_FragCoord.x > size || gl_FragCoord.y > size)
		{
			fragcolor = vec4(0.);
			return;
		}
	}

	if (pass == 0)
	{
		vec2 uv = gl_FragCoord.xy / size;
		if (mode == MODE_DFT)
		{
			float value = average(texture2D(tex, uv).rgb);
			fragcolor = vec4(value, value, value, 1.);
		}
		else if (mode == MODE_IFT)
			fragcolor = vec4(texture2D(tex, uv).bgr, 1.);
		else
		{
			uv = tex_coord;
			fragcolor = vec4(texture2D(tex, uv).bgr, 1.);
		}
	}
	else if (pass == 1)
	{
		if (mode == MODE_DFT)
		{
			fragcolor = vec4(0., 0., 0., 1.);
			for(float n = 0.; n < size; n++)
			{
				vec2 a = -6.2831853 * (gl_FragCoord.xy - .5 - float(size) / 2.) * n / float(size);
				vec2 xn = texelFetch(tex, ivec2(size - n - 0.5, gl_FragCoord.y), 0).xy;
				fragcolor.xy += cmul(xn, a.x);
			}
			fragcolor.xy /= 64.;
			fragcolor.xy = 1.0 / (1.0 + exp(-fragcolor.xy));
		}
		else if (mode == MODE_IFT)
		{
			fragcolor = vec4(0., 0., 0., 1.);
			for (float n = 0.; n < size; n++)
			{
				vec2 a = -6.2831853 * (gl_FragCoord.xy - .5) * n / float(size);
				vec2 xn = texelFetch(tex, ivec2(n + 0.5, gl_FragCoord.y), 0).xy;
				xn = -log((1. - xn) / xn);
				xn *= 64.;
				fragcolor.xy += cmul(xn, a.x);
			}
			fragcolor.xy /= size;
			fragcolor = vec4(vec3(length(fragcolor.xy)), 1.);
		}
		else
		{
			vec2 coord = tex_coord;
			coord.y = 1. - coord.y;
			fragcolor = texture2D(tex, coord).rgba;
		}
	}
}