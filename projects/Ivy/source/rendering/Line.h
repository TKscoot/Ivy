#pragma once
#include <glad/glad.h>

#include "Types.h"

namespace Ivy
{
	class Line
	{
	public:
		Line() {}
		Line(Vec3 start, Vec3 end);
		void SetMVP(Mat4 mvp)
		{
			mMVP = mvp;
		}

		void SetColor(Vec3 color)
		{
			mLineColor = color;
		}

		void SetWidth(float width)
		{
			mLineWidth = width;
			glLineWidth(mLineWidth);
		}

		void Draw(Mat4 mvp)
		{
			mMVP = mvp;
			glUseProgram(mShaderProgram);
			glUniformMatrix4fv(glGetUniformLocation(mShaderProgram, "MVP"), 1, GL_FALSE, &mMVP[0][0]);
			glUniform3fv(glGetUniformLocation(mShaderProgram, "color"), 1, &mLineColor[0]);

			glBindVertexArray(mVAO);
			glDrawArrays(GL_LINES, 0, 2);
		}

		~Line()
		{

			glDeleteVertexArrays(1, &mVAO);
			glDeleteBuffers(1, &mVBO);
			glDeleteProgram(mShaderProgram);
		}

	private:
		GLuint		  mShaderProgram;
		GLuint		  mVBO;
		GLuint		  mVAO;
		Vector<float> mVertices;
		Vec3          mStartPoint;
		Vec3          mEndPoint;
		Mat4          mMVP;
		Vec3          mLineColor;
		float		  mLineWidth;
	};
}