#include "Core/Base.hpp"
#include "Core/Input.hpp"
#include "Application.hpp"
#include "Utils/File.hpp"

#include "Editor/EditorLayer.hpp"

namespace Gate {

  EditorLayer::EditorLayer()
    : mEditorCameraController(Application::getWindow().getAspectRatio())
    , mPerspectiveCameraController(Vec3{16.9640, -9.0777, 40.3504}, 45.0f, Application::getWindow().getAspectRatio())
  {
    Logger::trace("EditorLayer: Constructor was called");
    const auto themeFilepath = "assets/themes/default.json";
    auto theme = Theme::Settings::load(themeFilepath);
    if (theme) {
      config.apply(*theme);
    } else {
      Logger::error("Unable to apply theme %s", themeFilepath);
    }

    mMaterial = Material::get("Default");
    config.andMesh = Mesh::load("assets/3D/objects/AndGate.obj");
    config.notMesh = Mesh::load("assets/3D/objects/NotGate.obj");
    config.orMesh  = Mesh::load("assets/3D/objects/OrGate.obj");
    config.xorMesh = Mesh::load("assets/3D/objects/XorGate.obj");
    mMesh = Mesh::load("assets/3D/objects/Cube.obj");

    config.pinMesh = mMesh;
    config.activeMaterial = Material::get("Active");
    config.activeMaterial->diffuseMap = Texture::color(0xFF'20'20'FF).build();
    config.activeMaterial->specularMap = Texture::color(0x20'20'20'FF).build();
    config.inactiveMaterial = Material::get("Inactive");
    config.inactiveMaterial->diffuseMap = Texture::color(0x60'60'60'FF).build();
    config.inactiveMaterial->specularMap = Texture::color(0x40'40'40'FF).build();

    auto texture = Texture::load("assets/2D/textures/components.png").build();
    auto atlas = TextureAtlas(texture, 512);
    config.andGate = atlas.get(0);
    config.orGate  = atlas.get(1);
    config.xorGate = atlas.get(2);
    config.notGate = atlas.get(3);
  }
  EditorLayer::~EditorLayer() {
    config.pinMesh = nullptr;
    config.activeMaterial = nullptr;
    config.inactiveMaterial = nullptr;
    config.andGate = SubTexture();
    config.orGate = SubTexture();
    config.xorGate = SubTexture();
    config.notGate = SubTexture();
    config.andMesh = nullptr;
    config.notMesh = nullptr;
    config.orMesh  = nullptr;
    config.xorMesh = nullptr;
  }
  void EditorLayer::onUpdate2D(Timestep ts) {
    (void)ts;

    mBoard.render(Application::getRenderer2D());

    auto height = Application::getWindow().getHeight();

    // Wire Draw
    if (mMode == Mode::WireDraw) {
      Wire{
        Point(mWireStartPosition / (f32)config.grid.cell.size),
        Point(mWireEndPosition   / (f32)config.grid.cell.size)
      }.render(Application::getRenderer2D());

      Application::getRenderer2D().drawCenteredQuad(mWireStartPosition, config.selector.size, config.selector.color);
      Application::getRenderer2D().drawCenteredQuad(mWireEndPosition, config.selector.size, config.selector.color);
    }

    // TODO: Move to UI
    switch (mMode) {
      case Mode::Select: {
        // Selector cursor
        Application::getRenderer2D().drawCenteredQuad(mSelectorPosition, config.selector.size, config.selector.color);
        const StringView text = " Click on the board to draw a line,\n or press \"c\" to insert a component!";
        const auto size = 16;
        Application::getRenderer2D().drawText(text, Vec2{size, height - 3.0f * size}, size, Color::BLACK);
      }  break;
      case Mode::Remove: {
        // Selector cursor
        Application::getRenderer2D().drawCenteredQuad(mSelectorPosition, config.selector.size, Color::RED);
        const StringView text = " Click on a component or wire to delete it!";
        const auto size = 16;
        Application::getRenderer2D().drawText(text, Vec2{size, height - 3.0f * size}, size, Color::BLACK);
      }  break;
      case Mode::WireDraw: {
        const StringView text = " Press <ESCAPE> to cancel wire drawing";
        const auto size = 16;
        Application::getRenderer2D().drawText(text, Vec2{size, height - 2 * size}, size, Color::BLACK);
      }  break;
      case Mode::AddComponent: {
        Application::getRenderer2D().drawCenteredQuad(mSelectorPosition, config.selector.size, Color::ORANGE);

        String text = " Click on board to add ";
        text += componentTypeToString(mComponentType);
        text += " component";
        const auto size = 16;
        Application::getRenderer2D().drawText(text, Vec2{size, height - 2 * size}, size, Color::BLACK);
      }  break;
    }
  }
  void EditorLayer::onUpdate3D(Timestep ts) {
    mPerspectiveCameraController.onUpdate(ts);
    Application::getRenderer3D().begin(mPerspectiveCameraController);
    mBoard.render(Application::getRenderer3D());
  }
  void EditorLayer::onUpdate(Timestep ts) {
    Application::getRenderer2D().begin(mEditorCameraController.getCamera());
    if (mRenderMode == RenderMode::_2D) {
      onUpdate2D(ts);
    } else {
      mMode = Mode::Select;
      onUpdate3D(ts);
    }
  }
  bool EditorLayer::onKeyPressedEvent(const KeyPressedEvent& event) {
    if (event.getKey() == Key::Escape) {
      switch (mMode) {
        case Mode::Select:
          // TODO: deselect
          break;
        case Mode::Remove:
          mMode = Mode::Select;
          break;
        case Mode::WireDraw: {
          mMode = Mode::Select;
        } break;
        case Mode::AddComponent: {
          mMode = Mode::Select;
        }  break;
      }
    } else {
      switch (mMode) {
        case Mode::Select:
          if (event.getKey() == Key::C) {
            mMode = Mode::AddComponent;
          } else if (event.getKey() == Key::D) {
            mMode = Mode::Remove;
          } else if (event.getKey() == Key::N) {
            mBoard.pushChip(Chip::create());
          }
          break;
        case Mode::Remove: {
          if (event.getKey() == Key::C) {
            mMode = Mode::AddComponent;
          }
        } break;
        case Mode::WireDraw: {
        } break;
        case Mode::AddComponent: {
          if (event.getKey() == Key::S) {
            mComponentType = ComponentType::Switch;
          } else if (event.getKey() == Key::N) {
            mComponentType = ComponentType::Not;
          } else if (event.getKey() == Key::A) {
            mComponentType = ComponentType::And;
          } else if (event.getKey() == Key::O) {
            mComponentType = ComponentType::Or;
          } else if (event.getKey() == Key::X) {
            mComponentType = ComponentType::Xor;
          }
        }  break;
      }
    }

    if (event.getKey() == Key::Minus) {
      if (mRenderMode == RenderMode::_2D) {
        config.grid.cell.size -= 1;
        config.grid.cell.size = config.grid.cell.size >= 5 ? config.grid.cell.size : 5;
        mBoard.onResize(Application::getWindow().getWidth(), Application::getWindow().getHeight());
      } else {
        config.grid.cell.size3d -= 0.05f;
        config.grid.cell.size3d = config.grid.cell.size3d >= 0.05f ? config.grid.cell.size3d : 0.05f;
      }
    }
    if (event.getKey() == Key::Equal) {
      if (mRenderMode == RenderMode::_2D) {
        config.grid.cell.size += 1;
        mBoard.onResize(Application::getWindow().getWidth(), Application::getWindow().getHeight());
      } else {
        config.grid.cell.size3d += 0.05f;
      }
    }

    if (event.getModifier() == KeyModifier::Control && event.getKey() == Key::S) {
      auto node = Serializer::Convert<Board>::encode(mBoard);
      auto content = node.toString();

      const auto filename = "save.json";

      Logger::info("Saving file %s", filename);
      Application::saveFile(filename, content);
    }

    #ifndef GATE_PLATFORM_WEB
      if (event.getModifier() == KeyModifier::Control && event.getKey() == Key::O) {
        loadFile("save.json");
      }
    #endif

    if (event.getKey() == Key::R) {
      switch (mRenderMode) {
        case RenderMode::_2D:
          mRenderMode = RenderMode::_3D;
          break;
        case RenderMode::_3D:
          mRenderMode = RenderMode::_2D;
          break;
      }
    }

    if (mRenderMode == RenderMode::_3D) {
      if (event.getKey() == Key::M) {
        config.minimap.cyclePosition();
      } else if (event.getKey() == Key::Comma) {
        config.minimap.spacePercent -= 0.05f;
      } else if (event.getKey() == Key::Period) {
        config.minimap.spacePercent += 0.05f;
      }
      config.minimap.spacePercent = glm::clamp(config.minimap.spacePercent, 0.5f, 0.95f);
    }

    #ifndef GATE_PLATFORM_WEB
      if (event.getKey() == Key::Q) {
        Application::get().quit();
      }
    #endif
    return false;
  }
  bool EditorLayer::onMouseButtonPressedEvent(const MouseButtonPressedEvent& event) {
    if (event.getButton() == MouseButton::Left) {
      mClicked = true;

      switch (mMode) {
        case Mode::Select: {
          // TODO: Check for collision
          Point mousePosition = Point(gridAlginPosition(mLastMousePosition) / (f32)config.grid.cell.size);
          
          bool interacted;
          if (mRenderMode == RenderMode::_2D) {
            interacted = mBoard.click(mousePosition);
            if (!interacted) {
            mMode = Mode::WireDraw;
            mWireStartPosition = gridAlginPosition(mLastMousePosition);
            mWireEndPosition = mWireStartPosition;
          }
          } else {
            u32 value = Application::getRenderer3D().readPixel((u32)mLastMousePosition.x, (u32)mLastMousePosition.y);
            Logger::trace("Click(%u, %u): Entity ID: %u", (u32)mLastMousePosition.x, (u32)mLastMousePosition.y, value);
            interacted = mBoard.click(value);
          }
        }  break;
        case Mode::Remove: {
          Point mousePosition = Point(gridAlginPosition(mLastMousePosition) / (f32)config.grid.cell.size);
          mBoard.removeWire(mousePosition);
          mBoard.removeComponent(mousePosition);
        }  break;
        case Mode::WireDraw: {
          // TODO: Check if wires intersect
          Point from = Point(mWireStartPosition / (f32)config.grid.cell.size);
          Point to = Point(mWireEndPosition / (f32)config.grid.cell.size);
          bool connected = false;
          switch (mBoard.pushWire({ from, to })) {
            case WirePushState::Valid:
              break;
            case WirePushState::Invalid:
              GATE_TODO("implement this: wire is invalid");
              break;
            case WirePushState::Connected:
              connected = true;
              break;
          }

          mMode = Mode::WireDraw;
          mWireStartPosition = mWireEndPosition;
          if (connected) {
            mMode = Mode::Select;
          }
        } break;
        case Mode::AddComponent: {
          auto position = Point(getGridAlignedMousePosition() / (f32)config.grid.cell.size);
          Component* component = nullptr;
          switch (mComponentType) {
            case ComponentType::Switch: {
              component = new SwitchComponent(position);
            } break;
            case ComponentType::Not: {
              component = new NotComponent(position);
            } break;
            case ComponentType::And: {
              component = new AndComponent(position);
            } break;
            case ComponentType::Or: {
              component = new OrComponent(position);
            } break;
            case ComponentType::Xor: {
              component = new XorComponent(position);
            } break;
          }
          if (!mBoard.pushComponent(component)) {
            Logger::trace("Component is already at position (%u, %u)", position.x, position.y);
            mBoard.removeComponent(position);
            if (!mBoard.pushComponent(component)) {
              Logger::error("The component should have been removed!!!");
            }
          }
        } break;
      }
    }
    return false;
  }
  bool EditorLayer::onMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event) {
    if (event.getButton() == MouseButton::Left) {
      mClicked = false;
    }
    return false;
  }
  bool EditorLayer::onMouseMoveEvent(const MouseMoveEvent& event) {
    Vec2 gridPosition = gridAlginPosition(event.toVec2());

    switch (mMode) {
      case Mode::Select:
        setSelectorPosition(gridPosition);
        break;
      case Mode::Remove: {
        setSelectorPosition(gridPosition);
        if (mClicked) {
          Point mousePosition = Point(gridAlginPosition(mLastMousePosition) / (f32)config.grid.cell.size);
          mBoard.removeWire(mousePosition);
          mBoard.removeComponent(mousePosition);
        }
      }  break;
      case Mode::WireDraw: {
        mWireEndPosition = gridPosition;

        Vec2 temp = glm::abs(mWireEndPosition - mWireStartPosition);
        if (temp.x > temp.y) {
          mWireEndPosition = Vec2{mWireEndPosition.x, mWireStartPosition.y};
        } else {
          mWireEndPosition = Vec2{mWireStartPosition.x, mWireEndPosition.y};
        }
      }  break;
      case Mode::AddComponent:
        setSelectorPosition(gridPosition);
        break;
    }
    mLastMousePosition = event.toVec2();
    return false;
  }

  Vec2 EditorLayer::gridAlginPosition(Vec2 position) {
    Vec2 offsetPosition = position + (config.grid.cell.size / 2.0f);
    return Vec2{
      u32(offsetPosition.x) - u32(offsetPosition.x) % config.grid.cell.size,
      u32(offsetPosition.y) - u32(offsetPosition.y) % config.grid.cell.size
    };
  }
  Vec2 EditorLayer::getGridAlignedMousePosition() {
    return gridAlginPosition(mLastMousePosition);
  }
  void EditorLayer::setSelectorPosition(Vec2 position) {
    if (mBoard.isValidPosition(Point(position / (f32)config.grid.cell.size))) {
      mSelectorPosition = position;
    }
  }
  void EditorLayer::onEvent(const Event& event) {
    if (mRenderMode == RenderMode::_3D) {
      if (mClicked) {
        mPerspectiveCameraController.onEvent(event);
      } else {
        mPerspectiveCameraController.resetLastPosition();
      }
    }
    

    event.dispatch(&EditorLayer::onWindowResizeEvent, this);
    event.dispatch(&EditorLayer::onKeyPressedEvent, this);
    event.dispatch(&EditorLayer::onMouseScrollEvent, this);
    event.dispatch(&EditorLayer::onMouseMoveEvent, this);
    event.dispatch(&EditorLayer::onMouseButtonPressedEvent, this);
    event.dispatch(&EditorLayer::onMouseButtonReleasedEvent, this);
    event.dispatch(&EditorLayer::onFileDropEvent, this);
  }
  void EditorLayer::loadFile(const String& path) {
    auto* content = Utils::fileToString(path.c_str());
    if (!content) {
      Logger::error("Couldn't read files contents");
      return;
    }
    auto node = Serializer::Json::parse(content);
    if (!node) {
      Logger::error("Unable to parse json file!");
      return;
    }
    free(content);
    Board newBoard;
    if (!Serializer::Convert<Board>::decode(*node, newBoard)) {
      Logger::error("Invalid board in json file");
      return;
    }
    Logger::trace("Replacing board");
    mBoard = newBoard;

    config.grid.cell.size = mBoard.getCurrentChip().getOptimalCellSize();
  }
  bool EditorLayer::onFileDropEvent(const FileDropEvent& event) {
    const auto& paths = event.getPaths();
    if (paths.size() != 1) {
      Logger::error("Dropping more tha one file is not allowed!");
      return true;
    }
    loadFile(paths[0]);
    return true;
  }
  bool EditorLayer::onWindowResizeEvent(const WindowResizeEvent& event) {
    auto[width, height] = event.getSize();
    mEditorCameraController.resize(width, height);
    mPerspectiveCameraController.resize(width, height);
    mBoard.onResize(width, height);
    return false;
  }
  bool EditorLayer::onMouseScrollEvent(const MouseScrollEvent& event) {
    if (mRenderMode == RenderMode::_3D) {
      mBoard.invalidateMiniMap(Application::getRenderer2D());
    }
    const auto x = event.getYOffset();
    if (x < 0) {
      config._3dZOffset += 0.5f;
      mBoard.moveCurrentChipDown();
    } else {
      config._3dZOffset -= 0.5f;
      mBoard.moveCurrentChipUp();
    }
    return false;
  }

} // namespace Gate
