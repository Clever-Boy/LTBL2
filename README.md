# ![LTBL2 Logo](http://i1218.photobucket.com/albums/dd401/222464/ltbllogosmall.png)

Let There Be Light 2
=======

A 2D dynamic soft shadows system with accurate penumbras/antumbras.

Install
-----------

LTBL2 relies only on SFML.

To get SFML, choose a package from here: [http://www.sfml-dev.org/download/sfml/2.2/](http://www.sfml-dev.org/download/sfml/2.2/)

LTBL2 uses CMake as the build system. You can get CMake here: [http://www.cmake.org/download/](http://www.cmake.org/download/)

Set CMake's source code directory to the LTBL2 root directory (the one that contains the /source folder as well as a CMakeLists.txt).

Set CMake's build directory to the same directory as in the previous step. Optionally, you can also set it to a folder of your choice, but this may make browse the source more difficult if you are using Visual Studio.

Then press configure, and choose your compiler.

It will likely error. If this happens, no fear, there is a fix!

You can specify the paths where CMake looks manually. They will appear in red if they need to be set in the CMake GUI.

SFML is a bit tricky, you have to add a custom variable entry for a variable called SFML_ROOT and set it to the SFML root directory.

When eventually the configuration does not result in errors you can hit generate. This will generate files necessary for your compiler.

You should then be able to compile and execute the program. If you are using Visual Studio, you may have to set your startup project to the ERL project, and you may have to add the source files to the project.

Quick Start
-----------

The first step is to include LTBL2's light system:

```cpp
#include <ltbl/lighting/LightSystem.h>
```

To use LTBL2, you must first load the resources LTBL2 requires. The resources are located in the resources directory.

You will need to load 2 SFML shader objects:

```cpp
sf::Shader unshadowShader;
sf::Shader lightOverShapeShader;
unshadowShader.loadFromFile("resources/unshadowShader.vert", "resources/unshadowShader.frag");
lightOverShapeShader.loadFromFile("resources/lightOverShapeShader.vert", "resources/lightOverShapeShader.frag");
```

You will also need to load a texture:

```cpp
sf::Texture penumbraTexture;
penumbraTexture.loadFromFile("resources/penumbraTexture.png");
penumbraTexture.setSmooth(true);
```

It is important that you set the texture filtering to smooth, otherwise it will look pixelated.

Now you can create the LightSystem object:

```cpp
ltbl::LightSystem ls;
ls.create(sf::FloatRect(-1000.0f, -1000.0f, 1000.0f, 1000.0f), window->getSize(), penumbraTexture, unshadowShader, lightOverShapeShader);
```

Where the first parameter is a starting region for the quadtree (doesn't need to be exact, it will automatically adjust itself!).
The second parameter is the size of the rendering region. This is typically the size of the window you ultimately want to apply the lighting to.
The other parameters are the resources we loaded earlier.

LTBL2 has 2 basic light types: Point emission and direction emission.
Point emission can be used for point lights and spot lights. Direction emission is mostly for sunlight.

To create a light, you will need to create a light mask texture. Two defaults, one for point and one for direction, are provided in the resources directory.
The light mask texture defines the shape of the light source.

LTBL2 lights use SFML sprites to render. So you will set the light's sprite to use your mask texture, and then properly set the sprite's origin, size, position, and rotation as is usual with SFML.

Below is an example for creating one point light and one directional light:

```cpp
std::shared_ptr<ltbl::LightPointEmission> light = std::make_shared<ltbl::LightPointEmission>();

light->_emissionSprite.setOrigin(<some_origin>);
light->_emissionSprite.setTexture(<mask_texture>);
light->_emissionSprite.setColor(<color>);
light->_emissionSprite.setPosition(<position_of_light>);
light->_localCastCenter = sf::Vector2f(0.0f, 0.0f); // This is where the shadows emanate from relative to the sprite

ls.addLight(light);

...

std::shared_ptr<ltbl::LightDirectionEmission> light = std::make_shared<ltbl::LightDirectionEmission>();

light->_emissionSprite.setTexture(<mask_texture>);
light->_castDirection = sf::Vector2f(<cast_direction>);

ls.addLight(light);
```

To create occluders, you must create a ltbl::LightShape object, and set the SFML shape it contains to represent the occluder:

```cpp
std::shared_ptr<ltbl::LightShape> lightShape = std::make_shared<ltbl::LightShape>();

lightShape->_shape.setPointCount(<number_of_points>);

for (int j = 0; j < fixedPoints.size(); j++)
	lightShape->_shape.setPoint(j, <point>);

lightShape->_shape.setPosition(<position>);

ls.addShape(lightShape);
```

More instructions to come. You are of course welcome to post on the SFML forum thread for help: [http://en.sfml-dev.org/forums/index.php?topic=16895.0](http://en.sfml-dev.org/forums/index.php?topic=16895.0)

------------------------------------------------------------------------------

LTBL2 uses the following external libraries:

SFML - source code is licensed under the zlib/png license.
