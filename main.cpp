#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <cstdlib>
#include <vector>
#include <limits>
#include <ctime> //https://cplusplus.com/reference/ctime/
using namespace std;
using namespace sf;

class Particle {
public:
    CircleShape shape;
    Vector2f velocity;
    float lifetime;

    // Constructor
    Particle(Vector2f position, Vector2f velocity, float lifetime)
        : velocity(velocity), lifetime(lifetime) {
        shape.setRadius(2.f);
        shape.setFillColor(Color::Yellow);
        shape.setPosition(position);
    }

};
struct PointText {
    //use struct for simple Data Containers
    Text text;            // SFML Text to display the points
    Vector2f velocity;    // Movement velocity for floating effect
    float lifetime;       // Lifetime in seconds
};
struct PowerUp {
    Sprite sprite;
    Vector2f velocity; // Movement velocity
    int type;          // Type of power-up (e.g., 1 = extra health, 2 = speed boost)
};


void spawnParticles(Vector2f position, vector<Particle>& particles) {
    const int numParticles = 20; // Number of particles per explosion
    for (int i = 0; i < numParticles; ++i) {
        // Randomize velocity
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.f; // Random angle in radians
        float speed = static_cast<float>(rand() % 100) / 20.f;            // Random speed
        Vector2f velocity(cos(angle) * speed, sin(angle) * speed);

        // Add the new particle using the Particle constructor
        particles.push_back(Particle(position, velocity, 1.0f)); // Lifetime is 1.0f
    }
}
void initializePowerUp(PowerUp& powerUp, const Texture& texture, int type, Vector2f position) {
    powerUp.sprite.setTexture(texture);
    powerUp.type = type;
    powerUp.sprite.setScale(0.2f, 0.2f);
    powerUp.sprite.setPosition(position);
    powerUp.velocity = Vector2f(0.f, 2.f); // Default downward movement
}
void clearGameObjects(vector<Sprite>& enemies, vector<Sprite>& bombs, vector<RectangleShape>& projectiles, vector<PointText>& pointTexts) {
    enemies.clear();
    projectiles.clear();
    bombs.clear();
    pointTexts.clear();
}
void spawnPowerUp(vector<PowerUp>& powerUps, const Texture& texture, int type, Vector2f position) {
    PowerUp powerUp;
    powerUp.sprite.setPosition(position);
    powerUp.velocity = Vector2f(0.f, 2.f); // Move downward
    initializePowerUp(powerUp, texture, type, position);
    powerUps.push_back(powerUp);
}
void showInstructions(RenderWindow& window, Font& font) {
    // Background Rectangle
    RectangleShape background(Vector2f(window.getSize().x, window.getSize().y));
    background.setFillColor(Color::Black); // Dark blue background

    // Title Text
    Text titleText;
    titleText.setFont(font);
    titleText.setCharacterSize(40);
    titleText.setFillColor(Color::White);
    titleText.setString("Instructions");
    titleText.setStyle(Text::Bold | Text::Underlined);
    titleText.setPosition(275,95);

    // Instruction Text
    Text instructionsText;
    instructionsText.setFont(font);
    instructionsText.setCharacterSize(24);
    instructionsText.setFillColor(Color::White);
    instructionsText.setString(
        "1. Use 'A' and 'D' to move left and right.\n"
        "2. Press 'Up' to shoot projectiles.\n"
        "3. Press 'Left' or 'Right' to shoot missiles.\n"
        "4. Avoid enemies and destroy them to earn points.\n"
        "5. Collect power-ups to gain health or speed.\n"
        "6. Game over if your health reaches zero.\n\n"
        "Press 'Escape' to return to the menu."
    );
    instructionsText.setPosition(70, 190);
    instructionsText.setLineSpacing(1.5) ;


    // Outer Glowing Border (Light Pink, Transparent)
    RectangleShape glowBorder(Vector2f(window.getSize().x - 80, window.getSize().y - 80));
    glowBorder.setPosition(40, 40);
    glowBorder.setFillColor(Color::Transparent);
    glowBorder.setOutlineThickness(15.f);
    glowBorder.setOutlineColor(Color(255, 0, 255, 80)); // Light pink (RGBA - semi-transparent)

    // Inner Border (Solid Pink)
    RectangleShape mainBorder(Vector2f(window.getSize().x - 100, window.getSize().y - 100));
    mainBorder.setPosition(50, 50);
    mainBorder.setFillColor(Color::Transparent);
    mainBorder.setOutlineThickness(8.f);
    mainBorder.setOutlineColor(Color(255, 0, 255)); // Hot pink border (solid)




    // Loop to display instructions
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                return; // Return to the main menu
        }

        window.clear();
        window.draw(background);   // Draw background
        window.draw(glowBorder);      // Draw glowing border (light pink)
        window.draw(mainBorder);      // Draw main border (hot pink)
        window.draw(titleText);   // Draw title
        window.draw(instructionsText); // Draw instructions
        window.display();
    }
}
void run_game() {//function for game
    const float PLAY_AREA_LEFT = 50.f;
    const float PLAY_AREA_RIGHT = 590.f; //define play area here so its easier to change
    const float PLAY_AREA_TOP = 30.f;
    const float PLAY_AREA_BOTTOM = 640.f;

    srand(time(NULL));
    RenderWindow window(VideoMode(640, 640), "Minigame");//create window
    window.setFramerateLimit(60);//frame rate limit
    
    bool isPaused = false;  // Track if the game is paused
    Clock pauseClock;       // Clock to handle pause duration
    float pauseDuration = 2.f; // Pause for 2 seconds between levels


    // Background setup
    Texture galaxyBackgroundTex;
    Sprite galaxyBackground;

    if (!galaxyBackgroundTex.loadFromFile("textures/galaxy4.png")) {
        throw "Could not load 'textures / galaxy4.png'\n";
    }
    galaxyBackground.setTexture(galaxyBackgroundTex);
    galaxyBackground.setScale(
        window.getSize().x / galaxyBackground.getGlobalBounds().width,
        window.getSize().y / galaxyBackground.getGlobalBounds().height
    );
    


    // Enemy setup
    Texture enemyTex;//initialize
    Texture enemy2Tex;
    Sprite enemy;
    if (!enemyTex.loadFromFile("textures/enemy_pink.png")) {//load texture
        throw "Could not load 'textures / enemy_pink.png'\n";//error handle
    }
    if (!enemy2Tex.loadFromFile("textures/enemy_pink2.png")) { // New texture for zigzag enemies
        throw "Could not load 'textures/enemy_pink2.png'\n";
    }
    enemy.setTexture(enemyTex);//assign texture
    enemy.setScale(Vector2f(0.09f, 0.09f));//resize
    int enemySpawnTimer = 40;
    vector<Sprite> enemies;//hold multiple
    float enemy_speed = 1.5f; // Initial enemy speed
    float oscillation_amp = 25.0f; // Amplitude of side-to-side movement
    float oscillation_frequency = 0.05f; // Frequency of oscillation
    float zigzag_amp = 50.0f; // Amplitude of zigzag
    float zigzag_direction = 1.0f; // Direction of zigzag (+1 or -1)





    // Projectile setup
    RectangleShape projectile(Vector2f(5.f, 5.f));
    projectile.setFillColor(Color::Green);
    vector<RectangleShape> projectiles;
    // Load sound for gun shot
    SoundBuffer gunShotBuffer;
    if (!gunShotBuffer.loadFromFile("sounds/laser5.wav")) {
        throw "Could not load 'sounds/laser5.wav'\n";
    }
    Sound gunShotSound;
    gunShotSound.setBuffer(gunShotBuffer);
    float projectileSpeed = 10.f; // Initial speed for projectiles

    
    //bomb 
    Texture bombTex;
    Sprite bomb;
    if (!bombTex.loadFromFile("textures/bomb.png")) {
        throw "Could not load 'textures/bomb.png'\n";
    }
    bomb.setTexture(bombTex); // Set the bomb texture
    bomb.setScale(.1f, .1f); // Adjust size of the bomb as needed
    vector<Sprite> bombs; // Store bombs
    float bombSpeed = -5.f; // Speed for bombs
    //explosion from bomb and enemy collision 
    vector<Particle> particles; // Store active particles
    
    //power ups from bomb and enemy collision 
    vector<PowerUp> powerUps; // Store active power-ups
    //initialize 
    Texture healthPowerUpTex, speedPowerUpTex;
    if (!healthPowerUpTex.loadFromFile("textures/health.png")) {
        throw "Could not load 'textures/health.png'\n";
    }
    if (!speedPowerUpTex.loadFromFile("textures/speed.png")) {
        throw "Could not load 'textures / speed.png'\n";
    }



    // Player setup
    Texture playerTex;
    Sprite player;
    if (!playerTex.loadFromFile("textures/ship(pink).png")) {
        throw "Could not load 'textures/ship(pink).png'\n";
    }
    player.setTexture(playerTex);
    player.setScale(Vector2f(0.2f, 0.2f));
    player.setPosition((PLAY_AREA_LEFT + PLAY_AREA_RIGHT) /2, PLAY_AREA_BOTTOM - 90.f);//initial position
    float player_speed = 8.f; // Initial player speed
    int* playerHealth = nullptr; //initialize to nullptr in case the game loop exits early
    playerHealth = new int(15);// Dynamically allocate memory for health
    int score = 0;

    // Health bar
    RectangleShape healthBar;
    healthBar.setFillColor(Color::Magenta);
    healthBar.setSize(Vector2f((float) *playerHealth * 20.f, 20.f));
    healthBar.setPosition(10.f, 10.f);

    // Load sound for health decrease
    SoundBuffer healthDecreaseBuffer;
    if (!healthDecreaseBuffer.loadFromFile("sounds/bomb.wav")) {
        throw "Could not load health decrease sound\n";
    }
    Sound healthDecreaseSound;
    healthDecreaseSound.setBuffer(healthDecreaseBuffer);

    // Score display
    Font basic_font;//set up font
    if (!basic_font.loadFromFile("fonts/orb.ttf"))//load font from file
        throw("couldn't load 'fonts/orb.ttf'");
    Text scoreText("Score: 0", basic_font, 20);
    scoreText.setPosition(10, 40);
    //points from projectile and enemy collision
    vector<PointText> pointTexts;// Store active point displays
    //PS: if declared inside the game loop it resets every frame


   //level display
    int level = 1; // Initialize the level
    Text levelText("Level: " + to_string(level), basic_font, 20);
    levelText.setFillColor(Color::White);
    levelText.setPosition(500, 10); // Adjust position as needed
    



    //load sound for final death
    SoundBuffer finalHitBuffer;
    if (!finalHitBuffer.loadFromFile("sounds/final_explosion.flac")) {
        throw "Could not load 'sounds/final_explosion.flac'\n";
    }
    Sound finalHitSound;
    finalHitSound.setBuffer(finalHitBuffer);

    // Game loop
    while (window.isOpen() && *playerHealth > 0) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
                return; // Return to menu
        }

        // Pause logic
        if (isPaused) {
            // Check if pause duration is over
            if (pauseClock.getElapsedTime().asSeconds() >= pauseDuration) {
                isPaused = false; // Resume the game
                enemySpawnTimer = 0; // Reset spawn timer to avoid immediate spawn
            }
            else {
                // Display "Level X" message
                Text levelMessage("Level " + to_string(level), basic_font, 50);
                levelMessage.setFillColor(Color::Magenta);
                levelMessage.setPosition(200, 250);

                window.clear();
                window.draw(galaxyBackground); // Background
                window.draw(levelMessage);    // Level message
                window.display();
                continue; // Skip the rest of the game loop
            }
        }



        // Player movement
        if (Keyboard::isKeyPressed(Keyboard::A) && player.getPosition().x > PLAY_AREA_LEFT) {
            player.move(-player_speed, 0.f); //player speed will change with levels
        }
        if (Keyboard::isKeyPressed(Keyboard::D) && player.getPosition().x < PLAY_AREA_RIGHT - player.getGlobalBounds().width) {
            player.move(player_speed, 0.f);
        }

       //fire from different spots
        if (Keyboard::isKeyPressed(Keyboard::Up) && projectiles.size() < 30) {
            projectile.setPosition(
                player.getPosition().x + player.getGlobalBounds().width / 2 - projectile.getSize().x / 2,
                player.getPosition().y
            );
            projectiles.push_back(RectangleShape(projectile));
            gunShotSound.play();
        }

        // Fire bombs from left and right
        if (Keyboard::isKeyPressed(Keyboard::Left) && bombs.size() < 1) {
            bomb.setPosition(
                player.getPosition().x + player.getGlobalBounds().width * 0.1f - bomb.getGlobalBounds().width / 2,
                player.getPosition().y
            );
            bombs.push_back(Sprite(bomb));
            gunShotSound.play();
        }

        if (Keyboard::isKeyPressed(Keyboard::Right) && bombs.size() < 1) {
            bomb.setPosition(
                player.getPosition().x + player.getGlobalBounds().width * 0.9f - bomb.getGlobalBounds().width / 2,
                player.getPosition().y
            );
            bombs.push_back(Sprite(bomb));
            gunShotSound.play();
        }

        // Move bombs
        for (size_t i = 0; i < bombs.size(); i++) {
            bombs[i].move(0.f, bombSpeed); // Move bombs upward
            if (bombs[i].getPosition().y < PLAY_AREA_TOP) { // Remove off-screen bombs
                bombs.erase(bombs.begin() + i);
            }
        }
        //move particles
        for (size_t i = 0; i < particles.size(); ++i) {
            particles[i].shape.move(particles[i].velocity);
            particles[i].lifetime -= 0.016f; // Assuming 60 FPS
            if (particles[i].lifetime <= 0) {
                particles.erase(particles.begin() + i);
                --i; // Adjust index after removal
            }
        }
        //move powerup
        for (size_t i = 0; i < powerUps.size(); i++) {
            powerUps[i].sprite.move(powerUps[i].velocity);

            // Remove power-ups if they go off-screen
            if (powerUps[i].sprite.getPosition().y > PLAY_AREA_BOTTOM) {
                powerUps.erase(powerUps.begin() + i);
                i--;
            }
        }



        for (size_t i = 0; i < pointTexts.size(); i++) {
            pointTexts[i].text.move(pointTexts[i].velocity); // Move upward
            pointTexts[i].lifetime -= 0.016f; // Assuming 60 FPS (1/60 ? 0.016)

            if (pointTexts[i].lifetime <= 0) {
                pointTexts.erase(pointTexts.begin() + i);
                --i; // Adjust index after removal
            }
        }



        // Move projectiles
        for (size_t i = 0; i < projectiles.size(); i++) {
            projectiles[i].move(0.f, -projectileSpeed);
            if (projectiles[i].getPosition().y < PLAY_AREA_TOP) {
                projectiles.erase(projectiles.begin() + i);// Remove off-screen projectiles
            }
        }


        // Spawn enemies
        if (enemySpawnTimer < 50) {
            enemySpawnTimer++;
        }
        if (enemySpawnTimer >= 50) {
            if (rand() % 2 == 0) { // 50% chance for normal oscillating enemy
                float spawnX = PLAY_AREA_LEFT + rand() % int((PLAY_AREA_RIGHT - PLAY_AREA_LEFT) - enemy.getGlobalBounds().width);
                spawnX = max(PLAY_AREA_LEFT, min(spawnX, PLAY_AREA_RIGHT - enemy.getGlobalBounds().width));
                enemy.setPosition(spawnX, PLAY_AREA_TOP);
                enemies.push_back(Sprite(enemy));
               
            }
            else { // 50% chance for zigzag enemy
                Sprite zigzagEnemy = enemy;
                zigzagEnemy.setTexture(enemy2Tex); // Apply zigzag enemy texture
                zigzagEnemy.setScale(Vector2f(0.09f, 0.09f)); // scale for zigzag enemies
                float spawnX = PLAY_AREA_LEFT + rand() % int((PLAY_AREA_RIGHT - PLAY_AREA_LEFT) - zigzagEnemy.getGlobalBounds().width);
                spawnX = max(PLAY_AREA_LEFT, min(spawnX, PLAY_AREA_RIGHT - zigzagEnemy.getGlobalBounds().width));

                zigzagEnemy.setPosition(spawnX, PLAY_AREA_TOP);
                enemies.push_back(zigzagEnemy);
            }
            enemySpawnTimer = 0;
        }



        // Move enemies
        for (size_t i = 0; i < enemies.size(); i++) {
            if (i % 2 == 0) { // Zigzag movement for every second enemy
                float zigzagOffset = zigzag_amp * zigzag_direction;
                enemies[i].move(zigzagOffset * 0.05f, enemy_speed); // Diagonal zigzag pattern
                if (enemies[i].getPosition().x <= PLAY_AREA_LEFT || enemies[i].getPosition().x >= PLAY_AREA_RIGHT - enemy.getGlobalBounds().width) {
                    zigzag_direction *= -1; // Reverse direction on hitting screen bounds
                }
            }
                else { // Oscillating movement for the first type of enemy
                    float oscillationOffset = sin(enemies[i].getPosition().y * oscillation_frequency) * oscillation_amp;
                    enemies[i].move(oscillationOffset * 0.1f, enemy_speed); // Horizontal oscillation with downward movement
                    // enemy speed is set and increased as the levels progress
                }

                if (enemies[i].getPosition().y > PLAY_AREA_BOTTOM) {
                    enemies.erase(enemies.begin() + i);
                    (*playerHealth)--; // Lose health if an enemy reaches the bottom
                    healthDecreaseSound.play(); // Play health decrease sound
                }
            }

        // Check collisions (projectiles vs enemies)
        PointText pt;
        pt.text.setFont(basic_font); // Use the game's font
        pt.text.setCharacterSize(20);
        pt.text.setFillColor(Color::Magenta);

        for (size_t i = 0; i < projectiles.size(); i++) {
            for (size_t j = 0; j < enemies.size(); j++) {
                Sprite &currentEnemy = enemies[j]; // Get the current enemy
                if (projectiles[i].getGlobalBounds().intersects(currentEnemy.getGlobalBounds())) {
                    if (currentEnemy.getTexture() == &enemy2Tex) {
                        score += 20; // Higher score for hitting zigzag enemies

                        pt.text.setString("+20");    // Adjust dynamically 
                        pt.text.setPosition(currentEnemy.getPosition()); // Enemy's position
                        pt.velocity = Vector2f(0, -1.f); // Float upward slightly
                        pt.lifetime = 1.5f; // Display for 1.5 seconds
                        pointTexts.push_back(pt);

                    }
                    else {
                        score += 10;

                        pt.text.setString("+10");    // Adjust dynamically
                        pt.text.setPosition(currentEnemy.getPosition()); // Enemy's position
                        pt.velocity = Vector2f(0, -1.f); // Float upward slightly
                        pt.lifetime = 1.5f; // Display for 1.5 seconds
                        pointTexts.push_back(pt);
                    }
                    projectiles.erase(projectiles.begin() + i);
                    enemies.erase(enemies.begin() + j);
                    break;
                }
            }
        }
        //collision bomb and enemies
        for (size_t i = 0; i < bombs.size(); i++) {
            for (size_t j = 0; j < enemies.size(); j++) {
                if (bombs[i].getGlobalBounds().intersects(enemies[j].getGlobalBounds())) {
                    spawnParticles(bombs[i].getPosition(), particles); // Spawn explosion

                    // spawn power up at enemy position
                    if (rand() % 2 == 0) {
                        spawnPowerUp(powerUps, healthPowerUpTex, 1, enemies[j].getPosition());
                    }
                    else {
                        spawnPowerUp(powerUps, speedPowerUpTex, 2, enemies[j].getPosition());
                    }



                    //erase bomb and enemy after processing 
                    bombs.erase(bombs.begin() + i);
                    enemies.erase(enemies.begin() + j);
                   // score += 30; // Increment score
                    break;
                }
            }
        }

        //collision with player and power up
        for (size_t i = 0; i < powerUps.size(); i++) {
            if (player.getGlobalBounds().intersects(powerUps[i].sprite.getGlobalBounds())) {
                if (powerUps[i].type == 1) { // Extra health
                    (*playerHealth) = min((*playerHealth) + 1, 15); // Cap at 15
                }
                else if (powerUps[i].type == 2) { // Speed boost
                    player_speed = 10.f; // Permanently increase speed for the level
                }

                //error check
                cout << "Power-up collected: Type " << powerUps[i].type << '\n';
                powerUps.erase(powerUps.begin() + i);
                i--;
            }

        }

        
        // Update difficulty based on score
        if (score >= level * 100) { // Level up every 100 points
            level++;
            enemy_speed += 0.5f; // Increase enemy speed
            projectileSpeed += 1.f;//increase speed of bullet as level increases
            player_speed += 0.5f; // Gradually increase player speed
            if (enemySpawnTimer > 20) {
                enemySpawnTimer -= 5; // Reduce spawn timer
            }
            levelText.setString("Level: " + to_string(level)); // Update level display
            //clear the screen before level change
            clearGameObjects(enemies, bombs, projectiles, pointTexts);

            // Trigger pause
            isPaused = true;
            pauseClock.restart();
        }



        // Check collisions (player vs enemies)
        for (size_t i = 0; i < enemies.size(); i++) {
            if (player.getGlobalBounds().intersects(enemies[i].getGlobalBounds())) {
                (*playerHealth)--;
                enemies.erase(enemies.begin() + i);
                healthDecreaseSound.play(); // Play health decrease sound
            }
        }

       

        // Check if the player is out of health
        if (*playerHealth <= 0) {
            delete playerHealth; // Free memory when the player health reaches 0
            playerHealth = nullptr; // Avoid dangling pointer
            finalHitSound.play();
            // Display "Game Over" in the same window
            Text gameOverText("GAME OVER", basic_font, 50);
            gameOverText.setFillColor(Color::Magenta);
            gameOverText.setPosition(130, 200);

            Text finalScoreText("Your Score: " + to_string(score), basic_font, 30);
            finalScoreText.setFillColor(Color::White);
            finalScoreText.setPosition(200, 300);

            Text restartText("Press R to Restart or ESC to Exit", basic_font, 20);
            restartText.setFillColor(Color::White);
            restartText.setPosition(140, 400);
            while (window.isOpen()) {
                while (window.pollEvent(event)) {
                    if (event.type == Event::Closed) {
                        window.close();
                    }
                    if (event.type == Event::KeyPressed) {
                        if (event.key.code == Keyboard::R) {
                            run_game(); // Restart the game
                            return;
                        }
                        if (event.key.code == Keyboard::Escape) {
                            return; // Exit to main menu
                        }
                    }
                }


                window.clear(Color::Black);
                // Draw the Game Over screen overlay
                window.draw(gameOverText);
                window.draw(finalScoreText);
                window.draw(restartText);

                window.display();
            }
        }
   

        // Update UI
        healthBar.setSize(Vector2f((float)(*playerHealth) * 20.f, 20.f));
        scoreText.setString("Score: " + to_string(score));

        // Render everything
        window.clear();
        window.draw(galaxyBackground); // Draw background
        window.draw(player);
        for (const auto& proj : projectiles) {
            window.draw(proj);
        }
        for (const auto& bomb : bombs) {
            window.draw(bomb);
        }
        for (const auto& particle : particles) {
            window.draw(particle.shape);
        }
        for (const auto& powerUp : powerUps) {
            window.draw(powerUp.sprite);
        }

        for (const auto& enemy : enemies) {
            window.draw(enemy);
        }
        for (const auto& pt : pointTexts) {
            window.draw(pt.text);
        }

        window.draw(healthBar);
        window.draw(scoreText);
        window.draw(levelText);
        window.display();
    }
    // Clean up after game loop ends
    if (playerHealth) {
        delete playerHealth; // Ensure no memory leaks
        playerHealth = nullptr;
    }
}


int main() {
    RenderWindow window(VideoMode(800, 600), "Main Menu");    //create a window
    window.setFramerateLimit(60); //set frame rate limit

    Font basic_font;//set up font
    if (!basic_font.loadFromFile("fonts/orb.ttf"))//load font from file
        throw("couldn't load 'fonts/orb.ttf'");
    Font pretty_font;//set up font
    if (!pretty_font.loadFromFile("fonts/NEWBOROU.TTF"))//load font from file
        throw("couldn't load 'fonts/NEWBOROU.TTF'");
    

    // Load sound for menu select
    SoundBuffer menuSelectBuffer;
    if (!menuSelectBuffer.loadFromFile("sounds/select.wav")) {
        throw "Could not load 'sounds/select.wav'\n";
    }
    Sound menuSelectSound;
    menuSelectSound.setBuffer(menuSelectBuffer);

    // Create menu options
    Text startGameText, instructionsText, exitText;

    // set up start game text
    startGameText.setFont(pretty_font);
    startGameText.setString("Start Game");
    startGameText.setCharacterSize(40);
    startGameText.setFillColor(Color::White);
    startGameText.setPosition(300, 200);

    // set up _ text
    instructionsText.setFont(pretty_font);
    instructionsText.setString("Instructions");
    instructionsText.setCharacterSize(40);
    instructionsText.setFillColor(Color::White);
    instructionsText.setPosition(300, 300);

    // set up exit text
    exitText.setFont(pretty_font);
    exitText.setString("Exit");
    exitText.setCharacterSize(40);
    exitText.setFillColor(Color::White);
    exitText.setPosition(300, 400);




    //main loop for window 
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            // Check for mouse clicks
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
                sf::Vector2i mousePosition = Mouse::getPosition(window);

                // Check if "Start Game" is clicked
                if (startGameText.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) {
                    menuSelectSound.play(); // Play menu select sound
                    run_game();
                }
                // Check if "Instructions" is clicked
                else if (instructionsText.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) {
                    menuSelectSound.play(); // Play menu select sound
                    showInstructions(window, basic_font); // Show the instructions screen
                }
                // Check if "Exit" is clicked
                else if (exitText.getGlobalBounds().contains(mousePosition.x, mousePosition.y)) {
                    menuSelectSound.play(); // Play menu select sound
                    window.close();
                }
            }
        }

        // Highlight the text when hovering
        Vector2i mousePosition = Mouse::getPosition(window);
        startGameText.setFillColor(startGameText.getGlobalBounds().contains(mousePosition.x, mousePosition.y) ? Color::Magenta : Color::White);
        instructionsText.setFillColor(instructionsText.getGlobalBounds().contains(mousePosition.x, mousePosition.y) ? Color::Magenta : Color::White);
        exitText.setFillColor(exitText.getGlobalBounds().contains(mousePosition.x, mousePosition.y) ? Color::Magenta : Color::White);

        // Render the menu
        window.clear(Color::Black);
        window.draw(startGameText);
        window.draw(instructionsText);
        window.draw(exitText);
        window.display();
    }

    return 0;
}