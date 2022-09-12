#pragma	once

#include "Core/Base.hpp"
#include "Core/Math.hpp"

namespace Game::Physics {

	struct Intersection {
		bool isIntersecting;
		Vec3 direction;

		f32 getDistance() {
			return glm::length(direction);
		}
	};

}