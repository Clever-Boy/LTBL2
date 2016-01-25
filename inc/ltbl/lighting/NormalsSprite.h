#pragma once

#include <ltbl/quadtree/QuadtreeOccupant.h>

namespace ltbl
{
    //! A sprite reaction to lights.

    class NormalsSprite : public QuadtreeOccupant
    {
    public:

        NormalsSprite() {}

        inline sf::FloatRect getAABB() const
        {
            return _normalsSprite.getGlobalBounds();
        }

    public:

        sf::Sprite _normalsSprite;
    };
}
