#include <SFML/Graphics.hpp>

#include <ltbl/lighting/LightSystem.h>

int main(void)
{
    sf::RenderWindow window;
    window.setVerticalSyncEnabled(true);
    window.create(sf::VideoMode(1280, 720), "LTBL - Examples - Basic", sf::Style::Close);

    sf::View view = window.getDefaultView();

    //----- Initialize the light system

    sf::RenderStates lightRenderStates;
    lightRenderStates.blendMode = sf::BlendMultiply;

    sf::Texture penumbraTexture;
    penumbraTexture.loadFromFile("assets/penumbraTexture.png");
    penumbraTexture.setSmooth(true);

    sf::Shader unshadowShader, lightOverShapeShader, normalsShader;
    unshadowShader.loadFromFile("assets/unshadowShader.frag", sf::Shader::Fragment);
    lightOverShapeShader.loadFromFile("assets/lightOverShapeShader.frag", sf::Shader::Fragment);
    normalsShader.loadFromFile("assets/normalsShader.frag", sf::Shader::Fragment);

    ltbl::LightSystem ls;
    ls.create(sf::FloatRect{{0.f, 0.f}, view.getSize()}, window.getSize(), penumbraTexture, unshadowShader, lightOverShapeShader, normalsShader);
    ls.normalsEnabled(true);

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

    sf::RectangleShape blocker;
    blocker.setSize({200.f, 50.f});
    blocker.setPosition(500.f, 300.f);
    blocker.setFillColor(sf::Color::Red);

    auto lightBlocker = ls.allocateShape();
    lightBlocker->_shape.setPointCount(4u);
    lightBlocker->_shape.setPoint(0u, {0.f, 0.f});
    lightBlocker->_shape.setPoint(1u, {0.f, blocker.getSize().y});
    lightBlocker->_shape.setPoint(2u, blocker.getSize());
    lightBlocker->_shape.setPoint(3u, {blocker.getSize().x, 0.f});
    lightBlocker->_shape.setPosition(blocker.getPosition());
    ls.addShape(lightBlocker);

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
        window.draw(blocker);
        window.draw(pointLight);

        // Lighting rendering

        ls.render(view, unshadowShader, lightOverShapeShader, normalsShader);

        sf::Sprite sprite(ls.getLightingTexture());
        window.draw(sprite, lightRenderStates);

        window.display();
    }

    return EXIT_SUCCESS;
}
