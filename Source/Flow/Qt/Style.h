#ifndef __FLOW_QT_STYLE_H__
#define __FLOW_QT_STYLE_H__

#include "API.h"

#include <QColor>
#include <QFont>

struct FLOW_API FlowUIStyle
{
    /// Background
    QColor background_color;
    QColor grid_color_0; // Light color
    QColor grid_color_1; // Dark color

    /// Links
    QColor link_color;
    QColor link_color_selected;

    /// Nodes
    QColor node_background_0; // Default color
    QColor node_background_1; // Input and Output nodes
    QColor node_border_0;
    QColor node_border_1;
    QColor node_border_selected_0;
    QColor node_border_selected_1;
    QColor node_shadow_color;
    QColor node_title_color;

    /// Pins
    QColor pin_color;
    QColor pin_outline_color;
    QColor pin_color_highlight;
    QColor pin_outline_color_highlight;
    QColor pin_text_color;
    int pin_radius;
    int pin_margin;

    /// General
    QFont node_font;

    static const FlowUIStyle& default_style();

    static QString stylize_text(const char* txt);

private:
    static FlowUIStyle* s_default;
};

#endif // __FLOW_QT_STYLE_H__
