// BalloonBuster.cpp : This file contains the 'main' function. Program execution begins and ends there.
//



#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <SFML/Audio.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Keyboard.hpp>

using namespace std;
using namespace sf;
using namespace sfp;

const double KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
    if (!tex.loadFromFile(filename)) {
        cout << "Could not load " << filename << endl;
    }
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main()
{
    RenderWindow window(VideoMode(800, 600), "Duck Hunter Game");
    World world(Vector2f(0, 0));
    int score(0);
    int arrows(5);

    PhysicsSprite& crossBow = *new PhysicsSprite();
    Texture cbowTex;
    LoadTex(cbowTex, "images/crossbow.png");
    crossBow.setTexture(cbowTex);
    Vector2f sz = crossBow.getSize();
    crossBow.setCenter(Vector2f(400,
        600 - (sz.y / 2)));

    PhysicsSprite arrow;
    Texture arrowTex;
    LoadTex(arrowTex, "images/arrow.png");
    arrow.setTexture(arrowTex);
    bool drawingArrow(false);

    PhysicsRectangle top;
    top.setSize(Vector2f(800, 10));
    top.setCenter(Vector2f(400, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    /*PhysicsRectangle left;
    left.setSize(Vector2f(10, 600));
    left.setCenter(Vector2f(5, 300));
    left.setStatic(true);
    world.AddPhysicsBody(left);
    */

    /*PhysicsRectangle right;
    right.setSize(Vector2f(10, 600));
    right.setCenter(Vector2f(795, 300));
    right.setStatic(true);
    world.AddPhysicsBody(right);
    */
    Texture duckTex;
    LoadTex(duckTex, "images/duck.png");
    PhysicsShapeList<PhysicsSprite> ducks;
        /*PhysicsSprite& duck = ducks.Create();
        duck.setTexture(duckTex);
        int x = 50 + ((700 / 5));
        Vector2f size = duck.getSize();
        duck.setCenter(Vector2f(x, 20 + (size.y / 2)));
        duck.setVelocity(Vector2f(0.25, 0));
        world.AddPhysicsBody(duck);
        bool drawingDuck(false);
        duck.onCollision =
            [&drawingArrow, &world, &arrow, &duck, &ducks, &score, &right]
            (PhysicsBodyCollisionResult result) {
            if (result.object2 == arrow) {
                drawingArrow = false;
                world.RemovePhysicsBody(arrow);
                world.RemovePhysicsBody(duck);
                ducks.QueueRemove(duck);
                score += 10;
            }
            if (result.object2 == right) {
                ducks.QueueRemove(duck);
            }
            };
       */
          
    top.onCollision = [&drawingArrow, &world, &arrow]
    (PhysicsBodyCollisionResult result) {
        drawingArrow = false;
        world.RemovePhysicsBody(arrow);
        };

    Font fnt;
    if (!fnt.loadFromFile("raleway.ttf")) {
        cout << "Could not load font." << endl;
        exit(3);
    }
    Clock clock;
    Time lastTime(clock.getElapsedTime());
    Time currentTime(lastTime);
    Clock spawnDuck;
    Clock spawnDuckClock;
    Time lastDuckSpawnTime = spawnDuckClock.getElapsedTime();
    const Time duckSpawnInterval = seconds(0.5);

    while ((arrows > 0) || drawingArrow) {
        currentTime = clock.getElapsedTime();
        Time deltaTime = currentTime - lastTime;
        long deltaMS = deltaTime.asMilliseconds();
        

        if (deltaMS > 9) {
            lastTime = currentTime;
            world.UpdatePhysics(deltaMS);
            //MoveCrossbow(crossBow, deltaMS);
            if (Keyboard::isKeyPressed(Keyboard::Space) &&
                !drawingArrow) {
                drawingArrow = true;
                arrows = arrows - 1;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, -1));
                world.AddPhysicsBody(arrow);

            }
            
            Time currentDuckSpawnTime = spawnDuckClock.getElapsedTime();
            if (currentDuckSpawnTime - lastDuckSpawnTime >= duckSpawnInterval) {
                PhysicsSprite& duck = ducks.Create();
                duck.setTexture(duckTex);
                Vector2f size = duck.getSize();
                duck.setCenter(Vector2f(50, 20 + (size.y / 2)));  // Spawn at the left side
                duck.setVelocity(Vector2f(0.25, 0));
                world.AddPhysicsBody(duck);

                lastDuckSpawnTime = currentDuckSpawnTime;
                if (duck.getCenter().x > window.getSize().x) {
                    world.RemovePhysicsBody(duck);
                    ducks.QueueRemove(duck);
                }
                duck.onCollision =
                    [&drawingArrow, &world, &arrow, &duck, &ducks, &score]
                    (PhysicsBodyCollisionResult result) {
                    if (result.object2 == arrow) {
                        drawingArrow = false;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                        score += 10;
                    }
                    };
            }

           
            window.clear();
            if (drawingArrow) {
                window.draw(arrow);
            }
            ducks.DoRemovals();
            for (PhysicsShape& duck : ducks) {
                window.draw((PhysicsSprite&)duck);
            }
            window.draw(crossBow);
            Text scoreText;
            scoreText.setString(to_string(score));
            scoreText.setFont(fnt);
            scoreText.setPosition(Vector2f(790 - GetTextSize(scoreText).x, 550));
            window.draw(scoreText);

            Text arrowCountText;
            arrowCountText.setString(to_string(arrows));
            arrowCountText.setFont(fnt);
            arrowCountText.setPosition(Vector2f(10, 550));
            window.draw(arrowCountText);
            //world.VisualizeAllBounds(window);


            window.display();

        }
    }


    window.display(); // this is needed to see the last frame
    Text gameOverText;
    gameOverText.setString("GAME OVER");
    gameOverText.setFont(fnt);
    sz = GetTextSize(gameOverText);
    gameOverText.setPosition(400 - (sz.x / 2), 300 - (sz.y / 2));
    window.draw(gameOverText);
    window.display();
    bool spaceKeyPressed = false;
    while (window.isOpen() && !spaceKeyPressed) {
        if (Keyboard::isKeyPressed(Keyboard::Space)) {
            if (!spaceKeyPressed) {
                spaceKeyPressed = true;
            }
            else {
                window.clear();
                window.close();
            }
        }
    
    }
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
