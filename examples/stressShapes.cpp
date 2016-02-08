#include <SFML/Graphics.hpp>

#include <ltbl/lighting/LightSystem.h>

#include <sstream>

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

    sf::Shader unshadowShader, lightOverShapeShader, normalsShader;
    unshadowShader.loadFromFile("assets/unshadowShader.frag", sf::Shader::Fragment);
    lightOverShapeShader.loadFromFile("assets/lightOverShapeShader.frag", sf::Shader::Fragment);
    normalsShader.loadFromFile("assets/normalsShader.frag", sf::Shader::Fragment);

    ltbl::LightSystem ls;
    ls.create(sf::FloatRect{{0.f, 0.f}, view.getSize()}, window.getSize(), penumbraTexture, unshadowShader, lightOverShapeShader, normalsShader);

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
        blockers[i].setFillColor(sf::Color{static_cast<sf::Uint8>(rand() % 256), static_cast<sf::Uint8>(rand() % 256), static_cast<sf::Uint8>(rand() % 256)});

        auto lightBlocker = ls.allocateShape();
        lightBlocker->_shape.setPointCount(4u);
        lightBlocker->_shape.setPoint(0u, {0.f, 0.f});
        lightBlocker->_shape.setPoint(1u, {0.f, blockers[i].getSize().y});
        lightBlocker->_shape.setPoint(2u, blockers[i].getSize());
        lightBlocker->_shape.setPoint(3u, {blockers[i].getSize().x, 0.f});
        lightBlocker->_shape.setPosition(blockers[i].getPosition());
        ls.addShape(lightBlocker);
    }

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
        clock.restart();
        ls.render(view, unshadowShader, lightOverShapeShader, normalsShader);
        time += clock.getElapsedTime();

        sf::Sprite sprite(ls.getLightingTexture());
        window.draw(sprite, lightRenderStates);

        // Counter
        globalTime += globalClock.getElapsedTime();
        globalClock.restart();

        if (globalTime  .asSeconds() >= 1.f) {
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
