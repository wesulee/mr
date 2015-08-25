#pragma once

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
	void update(void);
	void draw(Canvas&);
	void add(Entity*);
private:
	std::list<Entity*> list;
};
