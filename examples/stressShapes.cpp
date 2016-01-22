#include <SFML/Graphics.hpp>

#include <ltbl/lighting/LightSystem.h>

#define SHAPES_COUNT 500u

int main(void)
{
    sf::Vector2u screenSize{1280u, 720u};

    sf::RenderWindow window;
    window.setVerticalSyncEnabled(true);
    window.create(sf::VideoMode(screenSize.x, screenSize.y), "LTBL - Examples - Basic", sf::Style::Close);

    sf::View view = window.getDefaultView();

    srand(time(nullptr));

    //----- Initialize the light system

    sf::RenderStates lightRenderStates;
    lightRenderStates.blendMode = sf::BlendMultiply;

    sf::Texture penumbraTexture;
    penumbraTexture.loadFromFile("assets/penumbraTexture.png");
    penumbraTexture.setSmooth(true);

    sf::Shader unshadowShader;
    unshadowShader.loadFromFile("assets/unshadowShader.vert", "assets/unshadowShader.frag");

    sf::Shader lightOverShapeShader;
    lightOverShapeShader.loadFromFile("assets/lightOverShapeShader.vert", "assets/lightOverShapeShader.frag");

    ltbl::LightSystem ls;
    ls.create(sf::FloatRect{{0.f, 0.f}, view.getSize()}, window.getSize(), penumbraTexture, unshadowShader, lightOverShapeShader);

    //----- Add a light

    sf::CircleShape pointLight(4.f);
    pointLight.setFillColor(sf::Color::Blue);
    pointLight.setOrigin(0.5f * pointLight.getLocalBounds().width, 0.5f * pointLight.getLocalBounds().height);

    sf::Texture pointLightTexture;
    pointLightTexture.loadFromFile("assets/pointLightTexture.png");
    pointLightTexture.setSmooth(true);

    auto light = std::make_shared<ltbl::LightPointEmission>();
    light->_emissionSprite.setOrigin(pointLightTexture.getSize().x * 0.5f, pointLightTexture.getSize().y * 0.5f);
    light->_emissionSprite.setTexture(pointLightTexture);
    light->_emissionSprite.setScale(10.0f, 10.0f);
    light->_emissionSprite.setColor({255u, 230u, 200u});
    light->_emissionSprite.setPosition(100.0f, 100.0f);
    ls.addLight(light);

    //----- Add a square blocking light

    std::vector<sf::RectangleShape> blockers;
    blockers.resize(SHAPES_COUNT);

    for (unsigned i = 0u; i < SHAPES_COUNT; ++i) {
        blockers[i].setSize({10.f + (rand() % 30), 5.f + (rand() % 50)});
        blockers[i].setPosition(rand() % screenSize.x, rand() % screenSize.y);
        blockers[i].setFillColor(sf::Color{rand() % 256, rand() % 256, rand() % 256});

        auto lightBlocker = std::make_shared<ltbl::LightShape>();
        lightBlocker->_shape.setPointCount(4u);
        lightBlocker->_shape.setPoint(0u, {0.f, 0.f});
        lightBlocker->_shape.setPoint(1u, {0.f, blockers[i].getSize().y});
        lightBlocker->_shape.setPoint(2u, blockers[i].getSize());
        lightBlocker->_shape.setPoint(3u, {blockers[i].getSize().x, 0.f});
        lightBlocker->_shape.setPosition(blockers[i].getPosition());
        ls.addShape(lightBlocker);
    }

    //----- Program loop

    bool quit = false;
    while (!quit) {
        // Inputs

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                quit = true;
                break;

            case sf::Event::MouseMoved:
                light->_emissionSprite.setPosition(event.mouseMove.x, event.mouseMove.y);
                pointLight.setPosition(event.mouseMove.x, event.mouseMove.y);
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    quit = true;
                break;

            default:
                break;
            }
        }

        // Normal rendering

        window.clear(sf::Color::White);

        for (unsigned i = 0u; i < SHAPES_COUNT; ++i)
            window.draw(blockers[i]);

        window.draw(pointLight);

        // Lighting rendering

        ls.render(view, unshadowShader, lightOverShapeShader);

        sf::Sprite sprite(ls.getLightingTexture());
        window.draw(sprite, lightRenderStates);

        window.display();
    }

    return EXIT_SUCCESS;
}
