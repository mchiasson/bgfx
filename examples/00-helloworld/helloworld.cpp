/*
 * Copyright 2011-2018 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

#include <bx/uint32_t.h>
#include "common.h"
#include "bgfx_utils.h"
#include <vector>

#ifndef NUM_IMG_ROW
#define NUM_IMG_ROW 10
#endif

#ifndef RENDERER_VERTEX_MAX
#define RENDERER_VERTEX_MAX 65536
#endif

namespace
{
	struct Transform
	{
		float position[2] = { 0.0f, 0.0f };
		float angle = 0.0f;
	};

	struct TextureCoord
	{
		float uv[2];
	};

	struct Vertex
	{
		float a_position[2];
		float a_texcoord[2];
	};

class ExampleHelloWorld : public entry::AppI
{
public:
	ExampleHelloWorld(const char* _name, const char* _description)
		: entry::AppI(_name, _description)
	{
	}

	void init(int32_t _argc, const char* const* _argv, uint32_t _width, uint32_t _height) override
	{
		//Args args(_argc, _argv);

		m_width  = _width;
		m_height = _height;
		m_debug  = BGFX_DEBUG_TEXT;
		m_reset  = BGFX_RESET_VSYNC;

		bgfx::Init init;
		//init.type = bgfx::RendererType::OpenGL;
		init.type = bgfx::RendererType::Direct3D11;
		init.vendorId = BGFX_PCI_ID_NONE;
		init.resolution.width  = m_width;
		init.resolution.height = m_height;
		init.resolution.reset  = m_reset;
		bgfx::init(init);

		ms_decl
			.begin()
			.add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
			.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
			.end();

		static uint16_t indices[RENDERER_VERTEX_MAX / 4 * 6];
		for (uint16_t i = 0; i < RENDERER_VERTEX_MAX / 4; ++i)
		{
			indices[i * 6 + 0] = i * 4 + 0;
			indices[i * 6 + 1] = i * 4 + 1;
			indices[i * 6 + 2] = i * 4 + 2;
			indices[i * 6 + 3] = i * 4 + 2;
			indices[i * 6 + 4] = i * 4 + 3;
			indices[i * 6 + 5] = i * 4 + 0;
		}

		// Create vertex and index buffer objects for the batches
		VBO = bgfx::createDynamicVertexBuffer(RENDERER_VERTEX_MAX, ms_decl);
		IBO = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));

		program = loadProgram("default_vert", "default_frag");

		atlasHandle = loadTexture("textures/atlas.png");

		u_texture0 = bgfx::createUniform("u_texture0", bgfx::UniformType::Int1);

		// Set view 0 clear state.
		bgfx::setViewClear(0
			, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
			, 0x303030ff
			, 1.0f
			, 0
			);

		bgfx::setViewRect(0, 0, 0, uint16_t(m_width), uint16_t(m_height));

		TextureCoord textures[18 * 4];

		float u0 = 32.0 / 2048.0f;
		float v0 = 32.0 / 2048.0f;
		float v1 = v0 + (128.0 / 2048.0f);
		for (size_t i = 0; i < 10; ++i)
		{
			float u1 = u0 + (128.0 / 2048.0f);

			textures[i * 4 + 0].uv[0] = u0;
			textures[i * 4 + 0].uv[1] = v0;
			textures[i * 4 + 1].uv[0] = u0;
			textures[i * 4 + 1].uv[1] = v1;
			textures[i * 4 + 2].uv[0] = u1;
			textures[i * 4 + 2].uv[1] = v1;
			textures[i * 4 + 3].uv[0] = u1;
			textures[i * 4 + 3].uv[1] = v0;

			u0 = u1 + (64.0 / 2048.0f);
		}

		u0 = (32.0 / 2048.0f);
		v0 = v1 + (64.0 / 2048.0f);
		for (size_t i = 10; i < 18; ++i)
		{
			float u1 = u0 + (128.0 / 2048.0f);
			float v1 = v0 + (128.0 / 2048.0f);

			textures[i * 4 + 0].uv[0] = u0;
			textures[i * 4 + 0].uv[1] = v0;
			textures[i * 4 + 1].uv[0] = u0;
			textures[i * 4 + 1].uv[1] = v1;
			textures[i * 4 + 2].uv[0] = u1;
			textures[i * 4 + 2].uv[1] = v1;
			textures[i * 4 + 3].uv[0] = u1;
			textures[i * 4 + 3].uv[1] = v0;

			u0 = u1 + (64.0 / 2048.0f);
		}

		transforms.resize(NUM_IMG_ROW*NUM_IMG_ROW);
		vertexData.resize(NUM_IMG_ROW*NUM_IMG_ROW * 4);

		const float xspace = float(m_width) / (float(NUM_IMG_ROW) + 1.0f);
		const float ySpace = float(m_height) / (float(NUM_IMG_ROW) + 1.0f);

		for (int y = 0; y < NUM_IMG_ROW; ++y)
		{
			for (int x = 0; x < NUM_IMG_ROW; ++x)
			{
				int i = (x + (y*NUM_IMG_ROW));
				transforms[i].position[0] = xspace * (x + 1);
				transforms[i].position[1] = ySpace * (y + 1);
				vertexData[i * 4 + 0].a_texcoord[0] = textures[(i % 18) * 4 + 0].uv[0];
				vertexData[i * 4 + 0].a_texcoord[1] = textures[(i % 18) * 4 + 0].uv[1];
				vertexData[i * 4 + 1].a_texcoord[0] = textures[(i % 18) * 4 + 1].uv[0];
				vertexData[i * 4 + 1].a_texcoord[1] = textures[(i % 18) * 4 + 1].uv[1];
				vertexData[i * 4 + 2].a_texcoord[0] = textures[(i % 18) * 4 + 2].uv[0];
				vertexData[i * 4 + 2].a_texcoord[1] = textures[(i % 18) * 4 + 2].uv[1];
				vertexData[i * 4 + 3].a_texcoord[0] = textures[(i % 18) * 4 + 3].uv[0];
				vertexData[i * 4 + 3].a_texcoord[1] = textures[(i % 18) * 4 + 3].uv[1];
			}
		}
	}

	virtual int shutdown() override
	{
		bgfx::destroy(VBO);
		bgfx::destroy(IBO);
		bgfx::destroy(program);
		bgfx::destroy(atlasHandle);
		bgfx::destroy(u_texture0);

		// Shutdown bgfx.
		bgfx::shutdown();

		return 0;
	}

	bool update() override
	{
		if (!entry::processEvents(m_width, m_height, m_debug, m_reset, &m_mouseState) )
		{
			for (size_t i = 0; i < NUM_IMG_ROW*NUM_IMG_ROW; ++i)
			{
				const float c = cosf(transforms[i].angle);
				const float s = sinf(transforms[i].angle);
				const float w = 128 * 7.5f / float(NUM_IMG_ROW);
				const float hw = w * 0.5f;
				const float x0 = -hw;
				const float x1 = w - hw;
				const float cx0 = c * x0;
				const float cx1 = c * x1;
				const float sx1 = s * x1;
				const float sx0 = s * x0;

				vertexData[i * 4 + 0].a_position[0] = cx0 - sx0 + transforms[i].position[0];
				vertexData[i * 4 + 0].a_position[1] = sx0 + cx0 + transforms[i].position[1];

				vertexData[i * 4 + 1].a_position[0] = cx0 - sx1 + transforms[i].position[0];
				vertexData[i * 4 + 1].a_position[1] = sx0 + cx1 + transforms[i].position[1];

				vertexData[i * 4 + 2].a_position[0] = cx1 - sx1 + transforms[i].position[0];
				vertexData[i * 4 + 2].a_position[1] = sx1 + cx1 + transforms[i].position[1];

				vertexData[i * 4 + 3].a_position[0] = cx1 - sx0 + transforms[i].position[0];
				vertexData[i * 4 + 3].a_position[1] = sx1 + cx0 + transforms[i].position[1];

				transforms[i].angle += 0.05f;
			}

			bgfx::touch(0);
			bgfx::update(VBO, 0, bgfx::copy(vertexData.data(), vertexData.size() * sizeof(Vertex)));
			bgfx::setIndexBuffer(IBO);
			bgfx::setVertexBuffer(0, VBO, 0, vertexData.size());
			bgfx::setTexture(0, u_texture0, atlasHandle);
			bgfx::setState(state);
			bgfx::submit(0, program);
			bgfx::frame();
			return true;
		}

		return false;
	}

	entry::MouseState m_mouseState;

	uint32_t m_width;
	uint32_t m_height;
	uint32_t m_debug;
	uint32_t m_reset;

	bgfx::VertexDecl ms_decl;
	bgfx::DynamicVertexBufferHandle VBO;
	bgfx::IndexBufferHandle IBO;
	bgfx::ProgramHandle program;
	bgfx::TextureHandle atlasHandle;
	bgfx::UniformHandle u_texture0;
	uint64_t state = BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA) | BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_CULL_CW;

	std::vector<Transform> transforms;
	std::vector<Vertex> vertexData;

};

} // namespace

ENTRY_IMPLEMENT_MAIN(ExampleHelloWorld, "00-helloworld", "Initialization and debug text.");
