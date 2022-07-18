#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

namespace Game {
  
  class PerspectiveCamera {
  public:
    PerspectiveCamera(glm::vec3 position, f32 fov, f32 aspect, f32 zNear, f32 zFar)
      : position{position},
      forward{0.0f, 0.0f, 1.0f},
      up{0.0f, 1.0f, 0.0f},
      projection{glm::perspective(fov, aspect, zNear, zFar)}
    {
    }

    inline glm::mat4 getViewProjection() const {
      return this->projection * glm::lookAt(this->position, this->position + this->forward, this->up);
    }

  protected:
    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 up;

    glm::mat4 projection;
  };

  class OrthographicCamera {
  public:
    OrthographicCamera(f32 left, f32 right, f32 bottom, f32 top, f32 zNear = -1.0f, f32 zFar = 1.0f)
      : projection{glm::ortho(left, right, bottom, top, zNear, zFar)}, view{1.0f}
    {}

    void offsetPosition(const glm::vec3& offset) {
      this->setPosition(this->position + offset);
    }

    void setPosition(const glm::vec3& position) {
      this->position = position;
      glm::mat4 transform = glm::translate(
        glm::mat4(1.0f),
        this->position
      ) * glm::rotate(
        glm::mat4(1.0f),
        glm::radians(this->rotation),
        glm::vec3(0, 0, 1)
      );

      this->view = glm::inverse(transform);
    }

    inline glm::mat4 getProjectionView() const {
      return this->projection * this->view;
    }

  protected:
    glm::mat4 projection;
    glm::mat4 view;

    glm::vec3 position = {0.0f, 0.0f, 0.0f};
    f32       rotation = 0.0f;
  };

} // namespace Game
