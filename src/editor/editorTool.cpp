#include "editorTool.hpp"

editorTool::editorTool(fe::baseGameState *state, fe::gameWorld *editWorld) : m_editingWorld(editWorld), m_state(state), m_destroy(false)
    {
    }

void editorTool::destroy()
    {
        m_destroy = true;
    }

bool editorTool::destroyed() const
    {
        return m_destroy;
    }
