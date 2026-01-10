#ifndef IGRAPHICS_HPP
#define IGRAPHICS_HPP

#include <algorithm>
#include <unordered_map>
#include <string>
#include <cstdint>

enum event_type {
  CLOSED,
  RESIZED,
  FOCUS_LOST,
  FOCUS_GAINED,
  TEXT_ENTERED,
  KEY_PRESSED,
  KEY_RELEASED,
  MOUSE_WHEEL_SCROLLED,
  MOUSE_BUTTON_PRESSED,
  MOUSE_BUTTON_RELEASED,
  MOUSE_MOVED,
  MOUSE_MOVED_RAW,
  MOUSE_ENTERED,
  MOUSE_LEFT,
  EMPTY = 99
};

typedef struct s_event {
  event_type type;
  union {
    int keyCode;
    struct {
      int x, y;
      int button;
    } mouse;
    struct {
      int width, height;
    } window;
    int wheelDelta;
    std::uint32_t unicode;
  };
} t_event;

class IGraphics {
public:
  IGraphics(unsigned int height, unsigned int width);
  IGraphics(const IGraphics& obj) = delete;
  IGraphics& operator=(const IGraphics& obj) = delete;
  virtual ~IGraphics() = default;

  virtual t_event checkEvents() = 0;
  virtual void beginFrame() = 0;
  virtual void endFrame() = 0;
  virtual void loadAssets(const char** paths) = 0;
  virtual void drawText(float x, float y, int size, const char* text) = 0;
  virtual void drawAsset(float x, float y, float width, float height, int degrees,
                         const char* assetPath) = 0;
  virtual void drawButton(float x, float y, float width, float height, const char* text) = 0;

protected:
  float windowWidth;
  float windowHeight;
};

extern "C" {
IGraphics* init(unsigned int height, unsigned int width);

void cleanup(void* g);
t_event checkEvents(void* g);
void loadAssets(void* g, const char** paths);
void drawAsset(void* g, float x, float y, float width, float height, int degrees,
               const char* assetPath);
void drawButton(void* g, float x, float y, float width, float height, const char* text);
void drawText(void* g, float x, float y, int size, const char* text);
void beginFrame(void* g);
void endFrame(void* g);
}

#endif
