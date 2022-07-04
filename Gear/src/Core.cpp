#include <Gear/Core.hpp>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Gear {
	
	void Initialize() {
		// TODO: Check for error
		glfwInit();
	}

	void Terminate() {
		void glfwTerminate();	
	}

} // namespace Gear