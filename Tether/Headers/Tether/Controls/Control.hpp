#pragma once

#include <Tether/Common/Defs.hpp>
#include <Tether/Math/Types.hpp>

namespace Tether::Controls
{
    class TETHER_EXPORT Control
    {
    public:
        Control() = default;
        
        void SetX(float x);
        void SetY(float y);
        void SetWidth(float width);
        void SetHeight(float height);
        void SetForegroundColor(Math::Vector4f color);
        void SetBackgroundColor(Math::Vector4f color);
        [[nodiscard]] float GetX() const;
        [[nodiscard]] float GetY() const;
        [[nodiscard]] float GetWidth() const;
        [[nodiscard]] float GetHeight() const;
        [[nodiscard]] Math::Vector4f GetForegroundColor() const;
        [[nodiscard]] Math::Vector4f GetBackgroundColor() const;
    private:
        float x = 0, y = 0;
        float width = 0, height = 0;
        Math::Vector4f foreground;
        Math::Vector4f background;
    };
}
