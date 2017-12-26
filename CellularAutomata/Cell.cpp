#include "Cell.h"

Cell::Cell() {
	type = 0;
	health = 0;
}

Cell::Cell(int t, int h) {
	type = t;
	health = h;
};