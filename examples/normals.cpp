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
    ls.normalsTargetSetView(view);
    ls.normalsEnabled(true);

    //----- Add a light

    sf::CircleShape pointLightImage(4.f);
    pointLightImage.setFillColor(sf::Color::Blue);
    pointLightImage.setOrigin(0.5f * pointLightImage.getLocalBounds().width, 0.5f * pointLightImage.getLocalBounds().height);

    sf::Texture pointLightTexture;
    pointLightTexture.loadFromFile("assets/pointLightTexture.png");
    pointLightTexture.setSmooth(true);

    auto pointLight = std::make_shared<ltbl::LightPointEmission>();
    pointLight->_emissionSprite.setOrigin(pointLightTexture.getSize().x * 0.5f, pointLightTexture.getSize().y * 0.5f);
    pointLight->_emissionSprite.setTexture(pointLightTexture);
    pointLight->_emissionSprite.setScale(15.0f, 15.0f);
    pointLight->_emissionSprite.setColor({255u, 230u, 200u});
    pointLight->_emissionSprite.setPosition(100.0f, 100.0f);
    ls.addLight(pointLight);

    //----- Add a texture with normal mapping

    // A head
    sf::Texture headTexture;
    headTexture.loadFromFile("resources/head.png");
    headTexture.setSmooth(true);
    sf::Vector2f headTextureSize(headTexture.getSize().x, headTexture.getSize().y);

    sf::Texture headNormalsTexture;
    headNormalsTexture.loadFromFile("resources/head_NORMALS.png");

    sf::Sprite head(headTexture);
    head.setOrigin(headTextureSize / 2.f);
    head.setPosition(view.getCenter());

    // A background
    sf::Texture backgroundTexture;
    backgroundTexture.loadFromFile("resources/background.png");
    backgroundTexture.setSmooth(true);
    backgroundTexture.setRepeated(true);

    sf::Texture backgroundNormalsTexture;
    backgroundNormalsTexture.loadFromFile("resources/background_NORMALS.png");
    backgroundNormalsTexture.setRepeated(true);

    sf::RectangleShape background;
    background.setTexture(&backgroundTexture);
    background.setSize({1280.f, 720.f});
    background.setTextureRect({0, 0, 1280, 720});

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
                pointLight->_emissionSprite.setPosition(event.mouseMove.x, event.mouseMove.y);
                pointLightImage.setPosition(event.mouseMove.x, event.mouseMove.y);
                break;

            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape)
                    quit = true;
                break;

            default:
                break;
            }
        }

        // Rendering

        window.clear(sf::Color::White);
        ls.normalsTargetClear();

        background.setTexture(&backgroundTexture);
        window.draw(background);
        background.setTexture(&backgroundNormalsTexture);
        ls.normalsTargetDraw(background);

        head.setTexture(headTexture);
        window.draw(head);
        head.setTexture(headNormalsTexture);
        ls.normalsTargetDraw(head);

        window.draw(pointLightImage);

        // Lighting rendering

        ls.normalsTargetDisplay();
        ls.render(view, unshadowShader, lightOverShapeShader, normalsShader);

        // Mix lighting with plain image

        sf::Sprite sprite(ls.getLightingTexture());
        window.draw(sprite, lightRenderStates);
        window.display();
    }

    return EXIT_SUCCESS;
}
