#pragma once


enum class EntityResourceID : int {NONE=0, PLAYER};


class EntityResource {
public:
	EntityResource();
	explicit EntityResource(const EntityResourceID);
	virtual ~EntityResource() {}
	EntityResourceID getID(void) const;
protected:
	EntityResourceID id;
};


inline
EntityResource::EntityResource() : EntityResource(EntityResourceID::NONE) {
}


inline
EntityResource::EntityResource(const EntityResourceID _id) : id(_id) {
}


inline
EntityResourceID EntityResource::getID() const {
	return id;
}
