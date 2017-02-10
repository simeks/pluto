#include "Style.h"

FlowUIStyle* FlowUIStyle::s_default = nullptr;


const FlowUIStyle& FlowUIStyle::default_style()
{
    if (!s_default)
    {
        s_default = new FlowUIStyle();
        s_default->background_color = QColor::fromRgb(30, 30, 30);
        s_default->grid_color_0 = QColor::fromRgb(50, 50, 50);
        s_default->grid_color_1 = QColor::fromRgb(104, 104, 104);

        s_default->link_color = QColor::fromRgb(180, 180, 180);
        s_default->link_color_selected = QColor::fromRgb(255, 255, 255);

        s_default->node_background_0 = QColor::fromRgb(62, 62, 62);
        s_default->node_background_1 = QColor::fromRgb(20, 20, 20);
        s_default->node_border_0 = QColor::fromRgb(20, 20, 20);
        s_default->node_border_1 = QColor::fromRgb(62, 62, 62);
        s_default->node_border_selected_0 = QColor::fromRgb(255, 255, 255);
        s_default->node_border_selected_1 = QColor::fromRgb(255, 255, 255);
        s_default->node_shadow_color = QColor::fromRgb(0, 0, 0);
        s_default->node_title_color = QColor::fromRgb(218, 218, 218);

        s_default->pin_color = QColor::fromRgb(150, 150, 150);
        s_default->pin_outline_color = QColor::fromRgb(104, 104, 104);
        s_default->pin_color_highlight = QColor::fromRgb(255, 255, 255);
        s_default->pin_outline_color_highlight = QColor::fromRgb(104, 104, 104);
        s_default->pin_text_color = QColor::fromRgb(218, 218, 218);
        s_default->pin_radius = 5;
        s_default->pin_margin = 5;

        s_default->node_font = QFont();
    }
    return *s_default;
}
QString FlowUIStyle::stylize_text(const char* txt)
{
    QString qname = "";
    qname[0] = toupper((int)txt[0]);
    for (int i = 1; i < strlen(txt); ++i)
    {
        if (txt[i] == '_')
        {
            ++i;
            qname.append(toupper((int)txt[i]));
        }
        else
        {
            qname.append(txt[i]);
        }
    }
    return qname;
}
