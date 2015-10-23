#pragma once

#include "constants.h"
#include <list>


class Canvas;
class Entity;


// Visual Effects Manager
class VFXManager {
	VFXManager(const VFXManager&) = delete;
	void operator=(const VFXManager&) = delete;
public:
	VFXManager() = default;
	~VFXManager();
	void update(const Constants::float_type);
	void draw(Canvas&);
	void add(Entity*);
private:
	std::list<Entity*> list;
};
