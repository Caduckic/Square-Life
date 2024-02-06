#include <vector>
#include <ctime>
#include <iostream>
#include <cmath>
#include <sstream>

#include <windows.h>

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

class RectangleShape {
private:
    sf::RectangleShape shape {};
    sf::Vector2f dir;
    float speed {50.f};
    float trueAlpha {255};
public:

    RectangleShape(sf::Vector2f dir, sf::RenderTarget& target) : dir{dir} {
        shape.setFillColor(sf::Color(rand()%255+1, rand()%255+1, rand()%255+1));
        shape.setSize(sf::Vector2f(10.f,10.f));
        shape.setPosition(sf::Vector2f((target.getSize().x/2) - 50.f, (target.getSize().y/2) - 50.f));
    }
    RectangleShape(sf::Vector2f dir, sf::RenderTarget& target, sf::Vector2f pos) : dir{dir} {
        shape.setFillColor(sf::Color(rand()%255+1, rand()%255+1, rand()%255+1));
        shape.setSize(sf::Vector2f(10.f,10.f));
        shape.setPosition(pos);
    }

    virtual ~RectangleShape() = default;

    void move(float deltaTime) {
        shape.move(dir * speed * deltaTime);
    }

    void reduceAlpha(float deltaTime, int scale) {
        trueAlpha -= deltaTime * scale;
        sf::Color color = shape.getFillColor();
        color.a = (int)floor(trueAlpha);
        shape.setFillColor(color);
    }

    unsigned getAlpha() {
        return trueAlpha;
    }

    void render(sf::RenderTarget& target) {
        target.draw(shape);
    }

    sf::Vector2f getSize() {
        return shape.getSize();
    }

    sf::Vector2f getPosition() {
        return shape.getPosition();
    }

    void setPosition(sf::Vector2f newPos) {
        shape.setPosition(newPos);
    }

    sf::Vector2f getDir() {
        return dir;
    }

    void setDir(sf::Vector2f newDir) {
        dir = newDir;
    }
};


sf::Vector2f getRandomDir();

int WINAPI WinMain(HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow) {
    HWND myConsole = GetConsoleWindow();
    ShowWindow(myConsole,0);
    // seed random
    srand(time(NULL));

    sf::RenderWindow window (sf::VideoMode(300,180), "Square Life", sf::Style::Close | sf::Style::Titlebar);
    window.setFramerateLimit(60);

    HWND hwnd = window.getSystemHandle();
    SetWindowPos(hwnd, HWND_TOPMOST, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);

    sf::Event ev;
    sf::Clock deltaClock;
    sf::Time dt;

    sf::Font font;

    if (!font.loadFromFile("./BebasNeue-Regular.ttf"))
        std::cerr << "Failed to load font" << std::endl;

    sf::Text text;
    text.setFont(font);
    text.setString("NONE");
    text.setCharacterSize(24);

    std::vector<RectangleShape> rects;

    rects.push_back({getRandomDir(), window});

    //sf::Vector2f dir = getRandomDir();
    // float speed {30.f};

    while (window.isOpen()) {
        dt = deltaClock.restart();
        float deltaTime = dt.asSeconds();
        // std::cout << deltaTime << std::endl;
        if (deltaTime > 0.2f) deltaTime = 0.2f;
        // poll events
        while (window.pollEvent(ev)) {
            switch (ev.type)
            {
            case sf::Event::KeyPressed:
                if (ev.key.code == sf::Keyboard::Escape)
                    window.close();
                break;
            case sf::Event::Closed:
                window.close();
                break;
            }
        }
        // update
        std::vector<RectangleShape> rectsToAdd {};
        for (size_t i {0}; i < rects.size(); i++) {
            rects.at(i).move(deltaTime);
            sf::Vector2f shapePos = rects.at(i).getPosition();
            sf::Vector2f shapeSize = rects.at(i).getSize();
            sf::Vector2u windowSize = window.getSize();
            sf::Vector2f dir = rects.at(i).getDir();
            
            bool hit {false};
            sf::Vector2f newDir = dir;

            if (shapePos.x < 0.f) {
                dir.x = -dir.x;
                shapePos.x = 0.f;
                hit = true;
                newDir.y = -newDir.y;
                newDir.x = -newDir.x;
            }
            if (shapePos.x > windowSize.x - shapeSize.x){
                dir.x = -dir.x;
                shapePos.x = windowSize.x - shapeSize.x - 1.f;
                hit = true;
                newDir.y = -newDir.y;
                newDir.x = -newDir.x;
            }
            if (shapePos.y < 0.f) {
                dir.y = -dir.y;
                shapePos.y = 0.f;
                hit = true;
                newDir.x = -newDir.x;
                newDir.y = -newDir.y;
            }
            if (shapePos.y > windowSize.y - shapeSize.y) {
                dir.y = -dir.y;
                shapePos.y = windowSize.y - shapeSize.y - 1.f;
                hit = true;
                newDir.x = -newDir.x;
                newDir.y = -newDir.y;
            }

            if (hit) {
                rects.at(i).setPosition(shapePos);
                rects.at(i).setDir(dir);
                rectsToAdd.push_back({newDir, window, shapePos});
            }
            rects.at(i).reduceAlpha(deltaTime, rects.size());

            if (rects.at(i).getAlpha() == 0 || rects.at(i).getAlpha() > 255)
                rects.erase(rects.begin() + i);
        }
        for (auto newRect : rectsToAdd)
            rects.push_back(newRect);
        
        std::stringstream ss;
        ss << "Total Squares: " << rects.size();
        text.setString(ss.str());

        // render
        window.clear();

            // construct image
            for (auto rect : rects)
                rect.render(window);
            
            window.draw(text);

        window.display();
    }

    return 0;
}

sf::Vector2f getRandomDir() {
    float x {static_cast<float>(rand()% 2 + 1)};
    if (x > 1) x = -1;
    float y {static_cast<float>(rand()% 2 + 1)};
    if (y > 1) y = -1;
    return sf::Vector2f(x,y);
}