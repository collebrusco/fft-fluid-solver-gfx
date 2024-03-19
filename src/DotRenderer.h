#ifndef DOT_RENDERER_H
#define DOT_RENDERER_H

#include <flgl.h>
#include <flgl/glm.h>
#include <flgl/tools.h>
#include "../lib/sw/Stopwatch.h"
#include "Dotfield.h"
#include "FieldRenderer.h"

struct DotFieldRenderer : public FieldRenderer {
	Mesh<Vt_classic> quad;
	Shader df_shad;
	Texture df_tex;
	Stopwatch timer;

	Dotfield const& dotfield;

	DotFieldRenderer(Dotfield const&);
	void init() override final;
	void buffer_texture();
	void prepare() override final;
	void render() override final;
	void destroy() override final;
};

#endif
