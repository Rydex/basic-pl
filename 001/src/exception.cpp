#include "exception.h"
#include <stdexcept>

Exception::Exception(
  const Position& pos_start,
  const Position& pos_end,
  const std::string& message,
  const std::string& details
)
  : pos_start(pos_start), pos_end(pos_end), message(message), details(details) {}

std::string Exception::as_string() const {
  std::string result = message + ": " + details;
  result += "\nFile " + pos_start.get_fn() + ", line " + std::to_string(pos_start.get_ln() + 1);
  result += "\n\n" + string_with_arrows(
    pos_start.get_ftxt(),
    pos_start,
    pos_end
  );
  return result;
}

IllegalCharException::IllegalCharException(
  const Position& pos_start,
  const Position& pos_end,
  char ch
)
  : Exception(pos_start, pos_end, "Illegal Character", "'" + std::string(1, ch) + "'") {}

InvalidSyntaxException::InvalidSyntaxException(
  const Position& pos_start,
  const Position& pos_end,
  const std::string& details
)
  : Exception(pos_start, pos_end, "Invalid Syntax", details) {}

RTException::RTException(
  const std::optional<Context>& context,
  const Position& pos_start,
  const Position& pos_end,
  const std::string& details
): 
  Exception(pos_start, pos_end, "Runtime Error", details),
  context(context) {}

std::string RTException::as_string() const {
  std::string result = generate_traceback();
  result += "\n\n" + string_with_arrows(
    pos_start.get_ftxt(),
    pos_start,
    pos_end
  );

  return result;
}

std::string RTException::generate_traceback() const {
  std::string result = "";

  Position pos = pos_start;
  std::optional<Context> ctx = this->context;

  while(ctx) {
    result += "  File" + pos.get_fn() + ", line " + std::to_string(pos.get_ln() + 1)
           +  ", in " + ctx->display_name + "\n" + result;

    pos = ctx->parent_entry_pos.value();
    ctx = *ctx->parent.value();
  }

  return "traceback (most recent call last):\n" + result;
}

std::string string_with_arrows(
  const std::string& text,
  const Position& pos_start,
  const Position& pos_end
) {
  std::string result; // keep result as string

  // find last occurence of newline
  // from current index of position minus one all the way to the left
  size_t idx_start_temp = text.rfind('\n', pos_start.get_idx() - 1);
  // set index start to be 0 if idx_start_temp was an npos (meaning that \n wasnt found)
  // otherwise, set it to the first occurence of \n which is idx_start_temp
  size_t idx_start = (idx_start_temp == std::string::npos) ? 0 : idx_start_temp;
  // find first occurence of newline starting from the start of the index plus one
  size_t idx_end = text.find('\n', idx_start + 1);
  // if end is an npos, i.e the substring was not found, then the index end will just be
  // the length of the text
  if (idx_end == std::string::npos) idx_end = text.length();

  // determines how many lines the error spans
  int line_count = pos_end.get_ln() - pos_start.get_ln() + 1;

  // loop through the affected lines
  for (int i = 0; i < line_count; i++) {
    // extracts current line using idx_start and idx_end
    std::string line = text.substr(idx_start, idx_end - idx_start);
    
    // on the first line, it uses pos_start.get_col()
    // for lines that are not the first line, it starts at 0
    int col_start = (i == 0) ? pos_start.get_col() : 0;

    // on last line, it is pos_end.get_col()
    // otherwise, this will span the entire line
    int col_end = (i == line_count - 1) ? pos_end.get_col() : line.length() - 1;

    // bounds checking
    // this just ensures col end is within line length
    if (col_end < 0 || col_end >= (int)line.length()) {
      col_end = line.length() - 1;
    }

    // if column start is greater than column end, clamp column start to column end
    if (col_start > col_end) {
      col_start = col_end;
    }

    // build output string
    // appends line of code followed by newline
    result += line + '\n';
    // adds carets under problematic range
    // spaces pad up to col_start, caret spans from col_start to col_end
    result += std::string(col_start, ' ') + std::string(std::max(1, col_end - col_start), '^');

    // updat line indices
    // moves idx_start to idx_end which is next line
    // finds next newline/end of text
    idx_start = idx_end;
    if (idx_start < text.length()) {
      idx_end = text.find('\n', idx_start + 1);
      if (idx_end == std::string::npos) idx_end = text.length();
    }
  }

  // cleanup; replace tabs with spaces
  // so that it looks consistent
  std::string final_result;
  for (char c : result) {
    final_result += (c == '\t') ? ' ' : c;
  }

  return final_result;
}