#include "Dotfield.h"
#include <flgl/logger.h>
#include <flgl.h>
using namespace glm;
LOG_MODULE(Dotfield);

uint32_t Dotfield::x() const {return _x;}
uint32_t Dotfield::y() const {return _y;}
uint32_t Dotfield::size() const {return _size;}
uint8_t* Dotfield::pixels() const {return _pixels;}
Dotfield::Dotfield(uint32_t x, uint32_t y) : lookup(x,y) {
	_x = x; _y = y; _size = x*y;
	_pixels = new uint8_t[3*_size];
}
Dotfield::~Dotfield(){delete [] _pixels;}

void Dotfield::user_input() {
	static auto const& mouse = window.mouse;
	static auto const& df = *(this);
	static uint32_t brush_size = 5;
	if (window.keyboard[GLFW_KEY_1].pressed)
		brush_size = 1;
	else if (window.keyboard[GLFW_KEY_2].pressed)
		brush_size = 2;
	else if (window.keyboard[GLFW_KEY_3].pressed)
		brush_size = 3;
	else if (window.keyboard[GLFW_KEY_4].pressed)
		brush_size = 4;
	else if (window.keyboard[GLFW_KEY_5].pressed)
		brush_size = 5;
	else if (window.keyboard[GLFW_KEY_6].pressed)
		brush_size = 6;
	else if (window.keyboard[GLFW_KEY_7].pressed)
		brush_size = 7;
	else if (window.keyboard[GLFW_KEY_8].pressed)
		brush_size = 8;
	else if (window.keyboard[GLFW_KEY_9].pressed)
		brush_size = 33;

	static uint8_t r = 0xFC, g = 0xF8, b = 0xFF;
	static DotType dtype = DT_STONE;
	if (window.keyboard[GLFW_KEY_H].pressed) {
		dtype = DT_STONE;
	}
	else if (window.keyboard[GLFW_KEY_J].pressed) {
		dtype = DT_SAND;
	}
	else if (window.keyboard[GLFW_KEY_K].pressed) {
		dtype = DT_WATER;
	}
	else if (window.keyboard[GLFW_KEY_L].pressed) {
		dtype = DT_GAS;
	}

	if (mouse.left.pressed && window.keyboard[GLFW_KEY_V].down) {df.explode(df.mouse_pos().x,df.mouse_pos().y,30,50);return;}

	if (mouse.left.down && !window.keyboard[GLFW_KEY_V].down) {
		if (window.keyboard[GLFW_KEY_SPACE].down) {
			ivec2 mpos = df.mouse_pos();
			for (int i = 0; i < brush_size; i++) {
				for (int j = 0; j < brush_size; j++) {
					if ((mpos.x-(brush_size/2)+i) < 0 || (mpos.y-(brush_size/2)+j) < 0) continue;
					if ((mpos.x-(brush_size/2)+i) > df.x() || (mpos.y-(brush_size/2)+j) > df.y()) continue;
					if (!df.lookup.empty(mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j))
						df.kill_dot((mpos.x-(brush_size/2)+i), (mpos.y-(brush_size/2)+j));
				}
			}
		} else {
		ivec2 mpos = df.mouse_pos();
			for (int i = 0; i < brush_size; i++) {
				for (int j = 0; j < brush_size; j++) {
					if ((mpos.x-(brush_size/2)+i) < 0 || (mpos.y-(brush_size/2)+j) < 0) continue;
					if ((mpos.x-(brush_size/2)+i) > df.x() || (mpos.y-(brush_size/2)+j) > df.y()) continue;
					if (df.lookup.empty(mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j)) {
						// auto e = df.add_dot(mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j,r -(rand()&0xF),g -(rand()&0xF),b -(rand()&0xF), prop);
						// df.dots.addComp<DotResist>(e, {});
						df.add_dot_type(dtype, mpos.x-(brush_size/2)+i,mpos.y-(brush_size/2)+j);
					}
				}
			}
		}
	}
}

bool Dotfield::bounds(int x, int y) const {
	return (x > 0) && (x < this->x()) &&
		   (y > 0) && (y < this->y());
}

ivec2 Dotfield::mouse_pos() const {
	int x = static_cast<int>((window.mouse.pos.x / window.width) * this->x());
	int y = static_cast<int>(((window.height - window.mouse.pos.y) / window.height) * this->y());
	x = max(0,x); y = max(0,y);
	return {x,y};
}

void Dotfield::color_texture(uint32_t i, uint8_t * clr) {
	// if (i < 0 || i > _size) {
	// 	LOG_ERR("accessing pixel buffer at %d, outside of %d size", i, _size); return;
	// }
	i = 3*i;
	memcpy(&_pixels[i], clr, 3);
}

void Dotfield::erase_dot(entID e) {
	Dot& d = dots.getComp<Dot>(e);
	static uint8_t z[] = {0x00,0x00,0x00,0x00};
	color_texture((d.y * x()) + d.x, z);
}

void Dotfield::add_dot_type(DotType type, uint32_t x, uint32_t y) {
	uint8_t r = 0 - (rand()&0xF); uint8_t g = 0 - (rand()&0xF); uint8_t b = 0 - (rand()&0xF);
	entID e = 0xFFFFFFFFFFFFFFFF;
	if (x < 0 || x > this->x() || y < 0 || y > this->y()) return;
	switch (type) {
	case DT_STONE:
		e = add_dot(x, y, r+0x2F, g+0x2F, b+0x2F, DP_NONE);
		dots.addComp<DotResist>(e, 100);
		break;
	case DT_SAND:
		e = add_dot(x, y, r+0xFE, g+0xFC, b+0xFF, DP_NONE|DP_DOWN|DP_DOWN_SIDE);
		dots.addComp<DotResist>(e, 33);
		break;
	case DT_WATER:
		e = add_dot(x, y, r+0x0F, g+0x3F, b+0xFF, DP_NONE|DP_DOWN|DP_DOWN_SIDE|DP_SIDE);
		dots.addComp<DotResist>(e, 10);
		break;
	case DT_GAS:
		e = add_dot(x, y, r+0xF9, g+0xFD, b+0xFE, DP_NONE|DP_UP|DP_UP_SIDE|DP_SIDE);
		dots.addComp<DotResist>(e, 4);
		break;
	case DT_NONE:
		break;
	}
}

void Dotfield::exptr(int x1, int y1, int x2, int y2, uint32_t r, uint8_t p) {
	// auto st = Stepper(x1,y1,x2,y2);
	// auto it = st.begin();
	// it = it.operator++();
	// while (it != st.end()) {
	// 	auto vec = *it;
	for (auto vec : Stepper(x1,y1,x2,y2)) {
		// LOG_DBG("%d,%d",vec.x,vec.y);
		if (!bounds(vec.x, vec.y)) continue;
		int l2 = (vec.x - x1)*(vec.x - x1) + (vec.y - y1)*(vec.y - y1);
		if (l2>((int)r)*((int)r)) return;
		if (!lookup.empty(vec.x,vec.y)) {
			auto e = lookup.get(vec.x, vec.y);
			auto r = dots.getComp<DotResist>(e).resist;
			if (r<=p) {
				kill_dot(e);
			} else return;
		}
		// it.operator++();
	}
}

void Dotfield::explode(int x, int y, int rad, uint8_t pow) {
	int i = x-(rad), j = y-(rad);
	for (;j<y+(rad);j++)
		exptr(x,y,i,j,rad,pow);
	for (;i<x+(rad);i++)
		exptr(x,y,i,j,rad,pow);
	for (;j>=y-(rad);j--)
		exptr(x,y,i,j,rad,pow);
	for (;i>=x-(rad);i--)
		exptr(x,y,i,j,rad,pow);
}

void Dotfield::kill_dot(entID dot) {
	erase_dot(dot);
	auto& d = this->dots.getComp<Dot>(dot);
	this->lookup.erase(d.x, d.y);
	this->dots.removeEntity(dot);
}

void Dotfield::kill_dot(uint32_t x, uint32_t y) {
	if (this->lookup.empty(x,y)) return; 
	auto e = this->lookup.get(x,y);
	erase_dot(e);
	this->dots.removeEntity(e);
	this->lookup.erase(x, y);
}

void Dotfield::move_dot(entID dot, uint32_t x, uint32_t y) {
	DotMove& dm = dots.addComp<DotMove>(dot);
	// dm->from = {dots.getComp<Dot>(dot).x, dots.getComp<Dot>(dot).y};
	dm.to = {x,y};
}

void Dotfield::erase_dots() {
	for (auto e : dots.view<DotMove>()) {
		erase_dot(e);
	}
}

void Dotfield::update_dots() {
	for (auto e : this->dots.view<Dot>()) {
		auto& d = this->dots.getComp<Dot>(e);
		if (d.y < 1) continue;
		if (d.props & DP_DOWN && this->lookup.empty(d.x,d.y-1)){
			this->move_dot(e, d.x, d.y-1); continue;
		}
		int32_t a = rand() & 0x00000002; a--;
		if (d.props & DP_DOWN_SIDE) {
			if (d.x-a > 0 && d.x - a < this->x() && this->lookup.empty(d.x-a,d.y-1)) {
				this->move_dot(e, d.x-a, d.y-1); 
				continue;
			} 
			if (d.x+a > 0 && d.x+a < this->x() && this->lookup.empty(d.x+a,d.y-1)) {
				this->move_dot(e, d.x+a, d.y-1); 
				continue;
			}
		}
		if (d.props & DP_UP && this->lookup.empty(d.x,d.y+1)){
			this->move_dot(e, d.x, d.y+1); continue;
		}
		if (d.props & DP_UP_SIDE) {
			if (d.x-a > 0 && d.x - a < this->x() && this->lookup.empty(d.x-a,d.y+1)) {
				this->move_dot(e, d.x-a, d.y+1); 
				continue;
			} 
			if (d.x+a > 0 && d.x+a < this->x() && this->lookup.empty(d.x+a,d.y+1)) {
				this->move_dot(e, d.x+a, d.y+1); 
				continue;
			}
		}
		if (d.props & DP_SIDE) {
			if (d.x-a > 0 && d.x - a < this->x() && this->lookup.empty(d.x-a,d.y)) {
				this->move_dot(e, d.x-a, d.y); 
				continue;
			}
		}
	}
}

void Dotfield::commit_dots() {
	for (auto e : dots.view<DotMove>()) {
		auto& d = dots.getComp<Dot>(e);
		auto& dm = dots.getComp<DotMove>(e);
		{
			if (lookup.empty(dm.to.x, dm.to.y)) {
				lookup.erase(d.x, d.y);
				d.x = dm.to.x; d.y = dm.to.y;
				lookup.set({d.x,d.y}, e);
			}
		}
		dots.removeComp<DotMove>(e);
	}

}
#include <unordered_set>
void Dotfield::paint_dots() {
	// std::unordered_set<ivec2> debug;
	// size_t debugct = 0;
	// size_t totalct = 0;
	for (auto e : dots.view<Dot>()) {
		// totalct++;
		Dot& d = dots.getComp<Dot>(e);
		color_texture((d.y * x()) + d.x, d.color);
		// { //DEBUG
		// 	if (debug.find({d.x,d.y}) != debug.end()) {
		// 	}
		// 	debug.insert({d.x,d.y});
		// }
	}
	// { //deBUG
	// 	static size_t ab = 0;
	// 	if (totalct != debug.size()) {
	// 		LOG_ERR("dot mismatch! %d total, %d dist", totalct, debug.size());
	// 	}
	// }
	// debug.clear();
}

Dotfield::DotLookup::DotLookup(size_t x, size_t y) : w(x), h(y) {
	lookup = new entID[x*y];
	memset(lookup, 0xFF, x*y*sizeof(entID));
}

Dotfield::DotLookup::~DotLookup() {
	delete [] lookup;
}

size_t Dotfield::DotLookup::p2i(int x, int y) {
	return (y*w)+x;
}

void Dotfield::DotLookup::set(ivec2 pos, entID dot) {
	// lookup.insert({pos, dot});
	lookup[p2i(pos.x,pos.y)] = dot;
}
void Dotfield::DotLookup::erase(int x, int y) {
	// lookup.erase({x, y});
	lookup[p2i(x,y)] = 0xFFFFFFFFFFFFFFFF;
}
bool Dotfield::DotLookup::empty(int x, int y) {
	// return lookup.end() == lookup.find({x, y});
	return lookup[p2i(x,y)] == 0xFFFFFFFFFFFFFFFF;
}
entID Dotfield::DotLookup::get(int x, int y) {
	// return lookup[{x,y}];
	return lookup[p2i(x,y)];
}
void Dotfield::DotLookup::clear() {
	// lookup.clear();
	memset(lookup, 0xFF, w*h*sizeof(entID));
}


