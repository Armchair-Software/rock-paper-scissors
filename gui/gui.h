#ifndef GUI_H_INCLUDED
#define GUI_H_INCLUDED

namespace render {
class window;
}

namespace gui {

class clipboard;

/// Helper functions for consistently starting, stopping and restarting the GUI
void init(render::window &window);
void shutdown();
void restart(render::window &window);

}

#endif // GUI_H_INCLUDED
