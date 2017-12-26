#include <SFML\Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include "Cell.h"

int getX(int i, int collumns) {
	return i % collumns;
}

int getY(int i, int rows) {
	return i / rows;
}

int main() {
	
	// Counters
	int cycles = 0;
	int steps = 0;

	const int HEALTH_LIMIT = 20;
	const int REPRODUCE_THRESH = 15;

	const int WIDTH = 1500;
	const int HEIGHT = WIDTH / 2;
	const int ROWS = (HEIGHT / 5);
	const int COLUMNS = (WIDTH / 5);

	const int NO_OF_CELLS = COLUMNS * ROWS;
	Cell cells[NO_OF_CELLS];

	// Populate Array
	for (int i = 0; i < NO_OF_CELLS; i++) {
		// t = type
		// % out of 1000
		// 90% - Blank, 5% - Prey, 5% - Predator
		int t = rand() % 1000;
		if (t > 150) {
			cells[i].type = 2;
		} else if (t > 50) {
			cells[i].type = 0;
		} else {
			cells[i].type = 1;
		}
		cells[i].health = rand() % HEALTH_LIMIT + 1;
	}

	std::cout << "Columns: " << COLUMNS << std::endl;
	std::cout << "Rows: " << ROWS << std::endl;
	std::cout << "Cells: " << NO_OF_CELLS << std::endl;

	// "Why the hell are you loading arial from your C drive?" I hear you cry
	// Because I, am an idiot
	// And mainly because Visual Studio is a pain in the ass and I couldn't be bothered to find
	// the resource files folder or whatever and this was easiest at the time alright so just, shut up and leave me alone
	sf::Font font;
	if (!font.loadFromFile("C:\arial.ttf")) {
		std::cout << "Error loading font\n";
	}
	
	sf::Text counter;
	counter.setFont(font);
	counter.setCharacterSize(16);
	counter.setFillColor(sf::Color::White);

	sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Cells");
	window.setFramerateLimit(60);

	// Prey, Predator, Blank
	sf::Color colours[3] = { sf::Color::Blue, sf::Color::Red, sf::Color::Black};



	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		window.clear(); // Clear previous frame and begin rendering loop

		// Start with AI first. 
		for (int i = 0; i < NO_OF_CELLS; i++) {

			// Don't bother processing anything if the cell is blank
			if (cells[i].type == 2) {
				continue;
			}

			if (cells[i].type == 0) {
				int pos = i;		// I work via a separate variable rather than I so I can keep track of it easily after movement.
				int new_pos;		// This is the potential new position for the cell 
				int old_pos = -1;	// If the cell moves, I want to be able to keep track of the old position in case the cell reproduces
									// This is to save me checking for empty cells around it when it does, as I know if the cell has moved the previous cell will be
									// This does mean that if the cell couldn't move but there is an empty cell nearby it cannot reproduce

				int direction = rand() % 5; // Calulcate a random direction of movement

				// Because I work via the array index directly rather than an (X, Y) system that relates to the index,
				// I calculate the index first then calculate the screen (X, Y) later.
				switch (direction) {
				case 0: new_pos = i - COLUMNS;	// UP
						break;
				case 1: new_pos = i + 1;		// RIGHT
						break;
				case 2: new_pos = i + COLUMNS;	// DOWN
						break;
				case 3: new_pos = i - 1;		// LEFT
						break;
				}

				// Ensure cell is not trying to leave the grid *Tron music plays*
				if (!(new_pos > (NO_OF_CELLS - 1)  || new_pos < 0)) {
					if (cells[new_pos].type == 2) {
						cells[new_pos].type = cells[i].type;		// Move cell
						cells[new_pos].health = cells[i].health;	// Move health to new position
						cells[i].type = 2;							// Reset previous cell to blank, health can remain because blank cells health is never touched and if a new cell ever moves there it is overwritten
						old_pos = pos;	// Keep track of previous movement
						pos = new_pos;	// Update current position
					}
				}
				
				// GROW MY SON
				cells[pos].health += 1;

				// The Birds and The Bees
				if (cells[pos].health >= REPRODUCE_THRESH) {
					// Checks to see if the cell moved this step, if it didn't, not reproduction
					if (!(old_pos == -1)) {
						cells[old_pos].type = 0;							// Create new baby prey
						cells[old_pos].health = rand() % HEALTH_LIMIT + 1;	// Generate a random health
						cells[pos].health = 1;								// Reset previous cells health
					}
				}

			} else if (cells[i].type == 1) {

				// Same as above
				int pos = 1;
				int new_pos;
				int old_pos = -1;
				int attack;			// Attack is the attacked cell index

				int direction = rand() % 5;

				switch (direction) {
				case 0: new_pos = i - COLUMNS;	// UP
						break;
				case 1: new_pos = i + 1;		// RIGHT
						break;
				case 2: new_pos = i + COLUMNS;	// DOWN
						break;
				case 3: new_pos = i - 1;		// LEFT
						break;
				}

				// This works the same as the movement of the prey
				// Probably should condense it into a function
				// But it would have to have a million arguments so I'll wait until I create
				// a version inside a class
				if (!(new_pos > (NO_OF_CELLS - 1) || new_pos < 0)) {
					if (cells[new_pos].type == 2) {
						cells[new_pos].type = cells[i].type;
						cells[new_pos].health = cells[i].health;
						cells[i].type = 2;
						cells[i].health = 0;
						old_pos = pos;
						pos = new_pos;					
					}
				}

				// Increasing this value means predators die faster
				// Decreasing means they live longer
				cells[pos].health -= 1;

				if (cells[pos].health <= 0) {
					// D I E
					cells[pos].type = 2;
					cells[pos].health = 0;
				}

				// Pick a random direction to try and attack
				direction = rand() % 5;
				switch (direction) {
				case 0: attack = i - COLUMNS;
					break;
				case 1: attack = i + 1;
					break;
				case 2: attack = i + COLUMNS;
					break;
				case 3: attack = i - 1;
					break;
				}

				// Ensure attack range is valid
				if (!(attack > (NO_OF_CELLS - 1) || attack < 0)) {
					// You know the old saying, if it's not prey, you can't attack it
					if (cells[attack].type == 0) {
						cells[attack].type = 1;		// Create a new predator
						cells[attack].health = rand() % HEALTH_LIMIT + 1;

						// Originally the predators absorbed the preys health when they attacked but I prefer it without this rule
						// cells[pos].health += cells[attack].health;
					}
				}

			}

			steps++;

		}

		// Drawing loop
		// Simply iterate over the array, calculate the X and Y from the index
		// and then paint the cell the correct colour
		for (int i = 0; i < NO_OF_CELLS; i++) {
			sf::RectangleShape rect(sf::Vector2f(5, 5));
			rect.setPosition(sf::Vector2f(getX(i, COLUMNS) * 5, getY(i, ROWS) / 2 * 5));
			rect.setFillColor(colours[cells[i].type]);
			window.draw(rect);
		}

		cycles++;

		// Update cycle counter
		// Do an FPS counter?
		// Maybe
		counter.setString("Cycles: " +  std::to_string(cycles) + "\nSteps: " + std::to_string(steps));
		counter.setPosition(0, 0);
		window.draw(counter);
		window.display();
		
	}

	return 0;
}
