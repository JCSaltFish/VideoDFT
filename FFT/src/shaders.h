#ifndef __shaders_h__
#define __shaders_h__

#include <string>
#include <vector>

#include <GL/glew.h>

std::string LoadShader(const std::string& strShaderFilename);
GLuint CreateShader(GLenum eShaderType, const std::string& strShaderFile);
GLuint CreateProgram(const std::vector<GLuint>& shaderList);

const std::string vQuad =
{
	"#version 430\n"

	"out vec2 tex_coord;\n"

	"const vec4 quad[4] = vec4[]\n"
	"(\n"
	"	vec4(-1.0, 1.0, 0.0, 1.0),\n"
	"	vec4(-1.0, -1.0, 0.0, 1.0),\n"
	"	vec4(1.0, 1.0, 0.0, 1.0),\n"
	"	vec4(1.0, -1.0, 0.0, 1.0)\n"
	");\n"

	"void main()\n"
	"{\n"
	"	gl_Position = quad[gl_VertexID];\n"
	"	tex_coord = 0.5 * (quad[gl_VertexID].xy + vec2(1.0));\n"
	"}\n"
};

const std::string fQuad =
{
"// From: https://www.shadertoy.com/view/XtScWt\n"

"#version 430\n"

"layout(location = 0) uniform sampler2D tex;\n"

"layout(location = 1) uniform int pass = 0;\n"
"layout(location = 2) uniform int size;\n"
"layout(location = 3) uniform int mode = 0;\n"
"const int MODE_DFT = 1;\n"
"const int MODE_IFT = 2;\n"

"in vec2 tex_coord;\n"
"out vec4 fragcolor;\n"

"vec2 cmul (vec2 a,float b) { return mat2(a,-a.y,a.x) * vec2(cos(b),sin(b)); }\n"

"float average(vec3 v)\n"
"{\n"
"	return dot(v, vec3(1.0)) / 3.0;   \n"
"}\n"

"void main()\n"
"{\n"
"	if (mode != 0)\n"
"	{\n"
"		if (gl_FragCoord.x > size || gl_FragCoord.y > size)\n"
"		{\n"
"			fragcolor = vec4(0.);\n"
"			return;\n"
"		}\n"
"	}\n"

"	if (pass == 0)\n"
"	{\n"
"		vec2 uv = gl_FragCoord.xy / size;\n"
"		if (mode == MODE_DFT)\n"
"		{\n"
"			float value = average(texture2D(tex, uv).rgb);\n"
"			fragcolor = vec4(value, value, value, 1.);\n"
"		}\n"
"		else if (mode == MODE_IFT)\n"
"			fragcolor = vec4(texture2D(tex, uv).bgr, 1.);\n"
"		else\n"
"		{\n"
"			uv = tex_coord;\n"
"			fragcolor = vec4(texture2D(tex, uv).bgr, 1.);\n"
"		}\n"
"	}\n"
"	else if (pass == 1)\n"
"	{\n"
"		if (mode == MODE_DFT)\n"
"		{\n"
"			fragcolor = vec4(0., 0., 1., 1.);\n"
"			for(float n = 0.; n < size; n++)\n"
"			{\n"
"				vec2 a = -6.2831853 * (gl_FragCoord.xy - .5 - float(size) / 2.) * n / float(size);\n"
"				vec2 xn = texelFetch(tex, ivec2(size - n - 0.5, gl_FragCoord.y), 0).xy;\n"
"				fragcolor.xy += cmul(xn, a.x);\n"
"			}\n"
"			fragcolor.xy /= 64.;\n"
"			fragcolor.xy = 1.0 / (1.0 + exp(-fragcolor.xy));\n"
"		}\n"
"		else if (mode == MODE_IFT)\n"
"		{\n"
"			fragcolor = vec4(0., 0., 0., 1.);\n"
"			for (float n = 0.; n < size; n++)\n"
"			{\n"
"				vec2 a = -6.2831853 * (gl_FragCoord.xy - .5) * n / float(size);\n"
"				vec2 xn = texelFetch(tex, ivec2(n + 0.5, gl_FragCoord.y), 0).xy;\n"
"				xn = -log((1. - xn) / xn);\n"
"				xn *= 64.;\n"
"				fragcolor.xy += cmul(xn, a.x);\n"
"			}\n"
"			fragcolor.xy /= size;\n"
"			fragcolor = vec4(vec3(length(fragcolor.xy)), 1.);\n"
"		}\n"
"		else\n"
"		{\n"
"			vec2 coord = tex_coord;\n"
"			coord.y = 1. - coord.y;\n"
"			fragcolor = texture2D(tex, coord).rgba;\n"
"		}\n"
"	}\n"
"}\n"
};

#endif
