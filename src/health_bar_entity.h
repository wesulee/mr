#pragma once

#include "health_bar.h"
#include <cstdint>


class EntityHealthBar : public HealthBar {
public:
	EntityHealthBar(const KillableGameEntity*, const int);
	~EntityHealthBar() = default;
	void draw(Canvas&) override;
	void set(const int) override;
private:
	uint32_t damageTime = 0;
	int barWidth = 0;
	bool drawTest = false;	// needed since just damageTime is not enough
};
