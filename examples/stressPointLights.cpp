#include <SFML/Graphics.hpp>

#include <ltbl/lighting/LightSystem.h>

#include <sstream>

#define LIGHTS_COUNT 100u

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

    sf::Shader unshadowShader, lightOverShapeShader, normalsShader;
    unshadowShader.loadFromFile("assets/unshadowShader.frag", sf::Shader::Fragment);
    lightOverShapeShader.loadFromFile("assets/lightOverShapeShader.frag", sf::Shader::Fragment);
    normalsShader.loadFromFile("assets/normalsShader.frag", sf::Shader::Fragment);

    ltbl::LightSystem ls;
    ls.create(sf::FloatRect{{0.f, 0.f}, view.getSize()}, window.getSize(), penumbraTexture, unshadowShader, lightOverShapeShader, normalsShader);

    //----- Add a light

    sf::Texture pointLightTexture;
    pointLightTexture.loadFromFile("assets/pointLightTexture.png");
    pointLightTexture.setSmooth(true);

    for (unsigned i = 0u; i < LIGHTS_COUNT; ++i) {
        auto light = std::make_shared<ltbl::LightPointEmission>();
        light->_emissionSprite.setOrigin(pointLightTexture.getSize().x * 0.5f, pointLightTexture.getSize().y * 0.5f);
        light->_emissionSprite.setTexture(pointLightTexture);
        float scale = 0.3f + (rand() % 40) / 10.f;
        light->_emissionSprite.setScale(scale, scale);
        light->_emissionSprite.setColor({static_cast<sf::Uint8>(rand() % 256), static_cast<sf::Uint8>(rand() % 256), static_cast<sf::Uint8>(rand() % 256)});
        light->_emissionSprite.setPosition(rand() % screenSize.x, rand() % screenSize.y);
        ls.addLight(light);
    }

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

    //----- Efficiency counter
    sf::Clock globalClock, clock;
    sf::Time globalTime, time;
    sf::Text text;
    sf::Font font;
    font.loadFromFile("assets/monofur.ttf");
    text.setFont(font);
    text.setCharacterSize(20);
    text.setPosition(10.f, 10.f);
    text.setColor(sf::Color::White);

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

        // Lighting rendering
        clock.restart();
        ls.render(view, unshadowShader, lightOverShapeShader, normalsShader);
        time += clock.getElapsedTime();

        sf::Sprite sprite(ls.getLightingTexture());
        window.draw(sprite, lightRenderStates);

        // Counter
        globalTime += globalClock.getElapsedTime();
        globalClock.restart();

        if (globalTime.asSeconds() >= 1.f) {
            std::wstringstream str;
            str << L"Light rendering time per second: " << time.asMilliseconds() << L" ms" << std::endl;
            text.setString(str.str());

            globalTime -= sf::seconds(1.f);
            time = sf::Time::Zero;
        }

        window.draw(text);

        window.display();
    }

    return EXIT_SUCCESS;
}
