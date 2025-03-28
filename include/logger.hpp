#pragma once
#include <iostream>

#define ANSI_COLOR_RESET			 "\x1b[0m"
#define ANSI_COLOR_BLACK			 "\x1b[30m"
#define ANSI_COLOR_RED				 "\x1b[31m"
#define ANSI_COLOR_GREEN			 "\x1b[32m"
#define ANSI_COLOR_YELLOW			 "\x1b[33m"
#define ANSI_COLOR_BLUE				 "\x1b[34m"
#define ANSI_COLOR_MAGENTA			 "\x1b[35m"
#define ANSI_COLOR_CYAN				 "\x1b[36m"
#define ANSI_COLOR_WHITE			 "\x1b[37m"
#define ANSI_COLOR_BRIGHT_BLACK		 "\x1b[30;1m"
#define ANSI_COLOR_BRIGHT_RED		 "\x1b[31;1m"
#define ANSI_COLOR_BRIGHT_GREEN		 "\x1b[32;1m"
#define ANSI_COLOR_BRIGHT_YELLOW	 "\x1b[33;1m"
#define ANSI_COLOR_BRIGHT_BLUE		 "\x1b[34;1m"
#define ANSI_COLOR_BRIGHT_MAGENTA	 "\x1b[35;1m"
#define ANSI_COLOR_BRIGHT_CYAN		 "\x1b[36;1m"
#define ANSI_COLOR_BRIGHT_WHITE		 "\x1b[37;1m"
#define ANSI_BG_COLOR_BLACK			 "\x1b[40m"
#define ANSI_BG_COLOR_RED			 "\x1b[41m"
#define ANSI_BG_COLOR_GREEN			 "\x1b[42m"
#define ANSI_BG_COLOR_YELLOW		 "\x1b[43m"
#define ANSI_BG_COLOR_BLUE			 "\x1b[44m"
#define ANSI_BG_COLOR_MAGENTA		 "\x1b[45m"
#define ANSI_BG_COLOR_CYAN			 "\x1b[46m"
#define ANSI_BG_COLOR_WHITE			 "\x1b[47m"
#define ANSI_BG_COLOR_BRIGHT_BLACK	 "\x1b[40;1m"
#define ANSI_BG_COLOR_BRIGHT_RED	 "\x1b[41;1m"
#define ANSI_BG_COLOR_BRIGHT_GREEN	 "\x1b[42;1m"
#define ANSI_BG_COLOR_BRIGHT_YELLOW	 "\x1b[43;1m"
#define ANSI_BG_COLOR_BRIGHT_BLUE	 "\x1b[44;1m"
#define ANSI_BG_COLOR_BRIGHT_MAGENTA "\x1b[45;1m"
#define ANSI_BG_COLOR_BRIGHT_CYAN	 "\x1b[46;1m"
#define ANSI_BG_COLOR_BRIGHT_WHITE	 "\x1b[47;1m"
#define ANSI_STYLE_BOLD				 "\x1b[1m"
#define ANSI_STYLE_UNDERLINE		 "\x1b[4m"
#define ANSI_STYLE_REVERSED			 "\x1b[7m"

#define TRACE						 std::cout << ANSI_COLOR_GREEN
#define WARN						 std::cout << ANSI_COLOR_YELLOW
#define ERROR						 std::cerr << ANSI_COLOR_RED
#define FATAL						 std::cerr << ANSI_COLOR_BRIGHT_RED