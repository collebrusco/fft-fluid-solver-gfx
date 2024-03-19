#include "DotRenderer.h"

DotFieldRenderer::DotFieldRenderer(Dotfield const& f) : timer(ftime::MILLISECONDS), dotfield(f) {}
void DotFieldRenderer::init() {
	quad = DefaultMeshes::tile<Vt_classic>();
	df_shad = Shader::from_source("passthrough_vert", "tex");
	df_tex.create();
	df_tex.bind();
	df_tex.pixelate();
	df_tex.unbind();
}

void DotFieldRenderer::buffer_texture() {
	timer.reset_start();
	df_tex.bind();
	df_tex.alloc(GL_RGB32F, field.x(), field.y(), GL_RGB, GL_UNSIGNED_BYTE, field.pixels());
	double stop = timer.stop_reset();
	df_tex.unbind();
	// LOG_DBG("updating took %3f ms", stop);
}

void DotFieldRenderer::prepare() {
	this->buffer_texture();
}

void DotFieldRenderer::render() {
	df_tex.bind();
	df_shad.bind();
	timer.reset_start();
	gl.draw_mesh(quad);
	double stop = timer.stop_reset();
	df_tex.unbind();
	df_shad.unbind();
	// LOG_DBG("drawing took %3f ms", stop);
}

void DotFieldRenderer::destroy() {

}
