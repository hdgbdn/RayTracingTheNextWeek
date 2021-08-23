#ifndef SHAPE_H_
#define SHAPE_H_
#include "glad/glad.h"
#include "Shader.h"

namespace hdgbdn
{
	class FullScreenQuad
	{
	public:
		FullScreenQuad()
		{
			float quadVertices[] = {
		  -1.0f,  1.0f,  0.0f, 1.0f,
		  -1.0f, -1.0f,  0.0f, 0.0f,
		   1.0f, -1.0f,  1.0f, 0.0f,

		  -1.0f,  1.0f,  0.0f, 1.0f,
		   1.0f, -1.0f,  1.0f, 0.0f,
		   1.0f,  1.0f,  1.0f, 1.0f
			};
			glGenVertexArrays(1, &quadVAO);
			glGenBuffers(1, &quadVBO);
			glBindVertexArray(quadVAO);
			glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		}
		~FullScreenQuad()
		{
			glDeleteVertexArrays(1, &quadVAO);
			glDeleteBuffers(1, &quadVBO);
		}
		void Draw(Shader& shader, GLuint tex)
		{
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex);
			glBindVertexArray(quadVAO);
			shader.Use();
			glDrawArrays(GL_TRIANGLES, 0, 6);
		}
	private:
		unsigned int quadVAO, quadVBO;
	};
}


#endif