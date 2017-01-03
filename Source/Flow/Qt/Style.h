#ifndef __FLOW_QT_STYLE_H__
#define __FLOW_QT_STYLE_H__

#include <QColor>
#include <QFont>

struct FlowUIStyle
{
    /// Background
    QColor background_color;
    QColor grid_color_0; // Light color
    QColor grid_color_1; // Dark color

    /// Links
    QColor link_color;
    QColor link_color_selected;

    /// Nodes
    QColor node_background;
    QColor node_border;
    QColor node_border_selected;
    QColor node_shadow_color;
    QColor node_title_color;

    /// Pins
    QColor pin_color;
    QColor pin_outline_color;
    QColor pin_color_highlight;
    QColor pin_outline_color_highlight;
    QColor pin_text_color;
    int pin_radius;

    /// General
    QFont node_font;

    static const FlowUIStyle& default_style();

private:
    static FlowUIStyle* s_default;
};

#endif // __FLOW_QT_STYLE_H__
