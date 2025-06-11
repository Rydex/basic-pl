#include "position.h"

Position::Position(int idx, int ln, int col, const std::string& fn, const std::string& ftxt)
	: idx(idx), ln(ln), col(col), fn(fn), ftxt(ftxt) {}

Position& Position::advance(char cur_char) {
	idx++;
	col++;

	if(cur_char == '\n') {
		ln++;
		col = 0;
	}

	return *this;
}

Position Position::copy() const {
	return Position(idx, ln, col, fn, ftxt);
}

int Position::get_col() const { return col; }
int Position::get_idx() const { return idx; }
int Position::get_ln() const { return ln; }
std::string Position::get_fn() const { return fn; }
std::string Position::get_ftxt() const { return ftxt; }