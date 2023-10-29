#include <iostream>
#include <SFML/Graphics.hpp>


const float TARGET_FPS = 30.0f;

const int WIDTH = 1280;
const int HEIGHT = 800;

class Viewer {
    private:
        int frame_counter = 0;
        sf::Time zero_second = sf::seconds(0.0f);
        sf::Time render_time = sf::seconds(1.0f / 60.0f);

    public:
        int width = WIDTH;
        int height = HEIGHT;

    Viewer(){
    };

    int launch_window(){
        sf::RenderWindow window(
            sf::VideoMode(WIDTH, HEIGHT),
            "Chicken Window",
            // sf::Style::Default combines these three.
            sf::Style::Resize | sf::Style::Titlebar | sf::Style::Close
        );
        window.setFramerateLimit(TARGET_FPS);

        sf::Font font;
        if (!font.loadFromFile("Arial.ttf"))
            return EXIT_FAILURE;
        sf::Text text("Chicken text message\nBreakline", font, 20);
        text.setPosition(0, 0);

        sf::Event event;
        sf::Texture frame_texture;
        frame_texture.create(WIDTH, HEIGHT);
        sf::Sprite sprite;
        sprite.setTexture(frame_texture);
        sf::Clock clock;

            // Start the game loop
            while (window.isOpen())
            {
            sf::Time elapsed_time = clock.restart();
            // if (bus->ppu.is_emulation_run){

            //     if (bus->ppu.residual_time > zero_second){
            //         bus->ppu.residual_time -= elapsed_time;
            //     }
            //     else{
            //         bus->ppu.residual_time += render_time;
            //         do { bus->clock(); } while ( not bus->ppu.frame_complete);
            //         bus->ppu.frame_complete = false;
            //     }
            // }
            while (window.pollEvent(event))
            {
                switch (event.type) {
                    // Close window: exit
                    case sf::Event::Closed:
                        window.close();
                        break;

                    case sf::Event::KeyPressed:
                        switch (event.key.code){
                            case sf::Keyboard::C:
                                
                                break;
                            case sf::Keyboard::F:

                                
                                break;
                            case sf::Keyboard::R:
                                
                                break;
                            case sf::Keyboard::I:
                                
                                break;
                            case sf::Keyboard::N:
                                
                                break;
                            case sf::Keyboard::Space:
                        
                                break;
                            case sf::Keyboard::D:
                                window.close();
                                break;
                            case sf::Keyboard::Up:
                                
                                break;
                            case sf::Keyboard::Down:
                                
                                break;

                            case sf::Keyboard::Left:
                                
                                break;

                            case sf::Keyboard::Right:
                                
                                break;

                            default:
                                break;

                        }
                        break;

                    default:
                        break;
                }
            }
            // Clear screen
            window.clear();
    
            // Draw the sprite
            //frame_texture.update(pixels);
            sprite.setTexture(frame_texture);
            window.draw(sprite);

            // Draw ppu as shader.
            // draw_from_ppu_shader();
            // window.draw(ppu_image_va);
    
            // Draw the string
            window.draw(text);
    
            // Update the window
            window.display();
            frame_counter++;
        }
    return 0;
    };
};
