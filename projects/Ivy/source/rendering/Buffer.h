#pragma once
#include "Types.h"
#include "glad/glad.h"
#include "BufferLayout.h"
#include "environment/Log.h"

namespace Ivy
{
    class Buffer
    {
    public:
        virtual ~Buffer() = default;

		/*!
		 * Creates the buffer object
		 * 
		 */
        virtual void CreateBuffer() = 0;

		/*!
		 * Creates the buffer object
		 * 
		 * \param size size in bytes of the buffer
		 */
        virtual void CreateBuffer(uint32_t size) = 0;

		/*!
		 * Creates the buffer object
		 * 
		 * \param data The data that the buffer should contain
		 * \param size size in bytes of the buffer
		 */
        virtual void CreateBuffer(void* data, uint32_t size) = 0;

		/*!
		 * Binds the buffer so the GPU can use it
		 * 
		 */
        virtual void Bind() = 0;

		/*!
		 * Unbinds the buffer so the GPU can't use it anymore until it gets bound again
		 * 
		 */
        virtual void Unbind() = 0;

		/*!
		 * Sets the data of the buffer
		 * 
		 * \param data pointer to the data 
		 * \param size size of the data array in bytes
		 */
		virtual void SetBufferData(const void* data, uint32_t size) = 0;

		/*!
		 * Sets data of the buffer at a specific offset
		 * 
		 * \param offset offset in bytes where the specified data should be
		 * \param data pointer to the data
		 * \param size size of the data array in bytes
		 */
		virtual void SetBufferSubData(uint32_t offset, const void* data, uint32_t size) = 0;

		/*!
		 * Gets the OpenGL Handle
		 * 
		 * (Only use this if you know what you are doing!!!)
		 * 
		 * \return Returns the GLuint OpenGL Buffer ID
		 */
        GLuint GetHandle()
        {
            return mID;
        }

		void Destroy()
		{
			glDeleteBuffers(1, &mID);
			mID = 0;
		}

    protected:
        GLuint mID = 0;
    };

    class VertexBuffer : public Buffer
    {
    public:

		/*!
		* Vertex buffer constructor
		* 
		* Creates the buffer object
		*
		*/
        VertexBuffer();

		/*!		
		 * Vertex buffer constructor
		 * 
		 * Creates the buffer object
		 *
		 * \param size size in bytes of the buffer
		 */
        VertexBuffer(uint32_t size);

		/*!		
		 * Vertex buffer constructor
		 *
		 * Creates the buffer object
		 *
		 * \param data The data that the buffer should contain
		 * \param size size in bytes of the buffer
		 */
        VertexBuffer(void* vertices, uint32_t size);

        virtual ~VertexBuffer();


        virtual void CreateBuffer() override;
        virtual void CreateBuffer(uint32_t size) override;
        virtual void CreateBuffer(void* data, uint32_t size) override;

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetBufferData(const void* data, uint32_t size) override;
		virtual void SetBufferSubData(uint32_t offset, const void* data, uint32_t size) override;


	private:
    };

    class IndexBuffer : public Buffer
    {
    public:
		/*!
		* Index buffer constructor
		*
		* Creates the buffer object
		*
		*/
        IndexBuffer();

		/*!
		 * Index buffer constructor
		 *
		 * Creates the buffer object
		 *
		 * \param size size in bytes of the buffer
		 */
        IndexBuffer(uint32_t count);

		/*!
		 * Index buffer constructor
		 *
		 * Creates the buffer object
		 *
		 * \param indices The 32bit unsigned int pointer to the array that the buffer should contain
		 * \param count the count of the indices (size of the array)
		 */
        IndexBuffer(uint32_t* indices, uint32_t count);

        virtual ~IndexBuffer();

        virtual void CreateBuffer() override;
        virtual void CreateBuffer(uint32_t size) override;
        virtual void CreateBuffer(void* data, uint32_t size) override;

        virtual void Bind() override;
        virtual void Unbind() override;

        virtual void SetBufferData(const void* data, uint32_t size) override;
		virtual void SetBufferSubData(uint32_t offset, const void* data, uint32_t size) override;

		/*!
		 * Gets how many indices are stored in the buffer
		 * 
		 * \return returns uint32_t count of indices
		 */
        uint32_t GetCount() const
        {
            return mCount;
        }

    private:
        uint32_t mCount = 0;
    };

}
