#ifndef _BASIC_SHADER_H_
#define _BASIC_SHADER_H_

#include <GL/glew.h>

class BasicShader
{
public:
	BasicShader(void);
	~BasicShader(void);

	inline GLuint getVertexShader()   { return vertex_shader; };
	inline GLuint getFragmentShader() { return fragment_shader; };
	inline GLuint getProgram()        { return shader_program; };

private:
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint shader_program;

	static const char *vertex_shader_src;
	static const char *fragment_shader_src;

	GLuint createAndCompileShader(GLenum shader_type, const char* shader_src);
};

#endif
