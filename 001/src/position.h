#ifndef POSITION
#define POSITION

#include <string>

class Position {
private:
	int idx = 0;
	int ln = 0;
	int col = 0;
	std::string fn, ftxt;

public:
	Position(int idx, int ln, int col, const std::string& fn, const std::string& ftxt);

	Position& advance(char cur_char = '\0');

	Position copy() const;

	int get_idx() const;
	int get_ln() const;
	int get_col() const;
	std::string get_fn() const;
	std::string get_ftxt() const;
};

#endif