// Sepid Ebrahimi | CS221

#include "matcher.h"

char next_ch(char *string) { // Helper Function: returns the next char
	return *(string + sizeof(char));
}

char before_ch(char *string) { // Helper Function: returns the previous char
	return *(string - sizeof(char));
}

int escape_modified(char *pattern) { // checks if the previous char was backslash
	if(before_ch(pattern) == '\\')
		return 1;
	return 0;
}

int question_modified(char *pattern) { // checks if the next char is "?"
	if(next_ch(pattern)  == '?')
		return 1;
	return 0;
}

int plus_modified(char *pattern) { // checks if the next char is "+"
	if(next_ch(pattern) == '+')
		return 1;
	return 0;
}

int is_operator(char pattern){ //checks if the char is an operator, if not it returns 0
  if(pattern == '.'){
    return 1;
  }
  else if (pattern == '?'){
    return 1;
  }
  else if (pattern == '+'){
    return 1;
  }
  else if(pattern == '\\'){
  	return 1;
  }

  return 0;
}


/**
 * Returns true if partial_line matches pattern, starting from
 * the first char of partial_line.
 */
int matches_leading(char *line, char *pattern) {
	//Check direct match
	if(*line == *pattern && !is_operator(*pattern)) {
		return 1;
	}
	//Check wildcard match
	if(*pattern == '.' && !escape_modified(pattern)) {
		return 1;
	}
	//Check escaped operator match
	if(*line == *pattern && is_operator(*pattern) && escape_modified(pattern)) {
		return 1;
	}
	//Check escape character
	if(*pattern == '\\')
		return matches_leading(line, pattern + sizeof(char));
	//Check for absent case of ? operator
	if(question_modified(pattern))
		return 1;
	return 0;
}

/**
 * You may assume that all strings are properly null terminated 
 * and will not overrun the buffer set by MAXSIZE 
 *
 * Implementation of the rgrep matcher function
 */

int rgrep_matches(char *line, char *pattern) {
    int init = 0;

    if(*pattern == '\0'){ // when pattern hits the null
      if(*line == '\n'){ // if line hits the end
        pattern -= init*sizeof(char);
      }
      init = 0;
      return 1;
    }

    if(*line == '\0'){ // if line hits the null
      return 0;
    }
    if(*pattern == '.' && escape_modified(pattern)) { // if there is "\" followed by "."
		return 1;
	}


	if(matches_leading(line, pattern)) {
		if(plus_modified(pattern)) { // checks for "+" modifier | means: if the next char is "+"
			int prev_char = 1;
			if(*pattern == *(pattern + 2 * sizeof(char)))
				prev_char = 2;

			while(*(line + prev_char * sizeof(char)) == *pattern && !(*pattern == '.' && !escape_modified(pattern)))
				line += sizeof(char);
			if(*pattern == '.' && escape_modified(pattern)) {
				char entire = *line;
				while(*(line + prev_char * sizeof(char)) == entire)
					line += sizeof(char);			
			}
			if(*pattern == '.' && !escape_modified(pattern)) {
				while(!matches_leading(line + prev_char * sizeof(char), pattern + 2 * sizeof(char))){
					line += sizeof(char);
					if((*line == '\0' || *line == '\n') && *(pattern + 2 * sizeof(char)) != '\0')
						return 0;
				}
			}

			pattern += sizeof(char);
			init++;
		}

		if(question_modified(pattern)) {  // checks for "?" modifier | means: if the next char is "+"
			
			if (*pattern == '.' && !escape_modified(pattern) && escape_modified(pattern + 3 * sizeof(char)) && *line == *(pattern + 3 * sizeof(char)) && *(line + sizeof(char)) != *(pattern + 3 * sizeof(char))) { // first 3 if statements check if we have ".", following by "?", and previous char was not backslask
				init += 4;
				return rgrep_matches(line + sizeof(char), pattern + 4 * sizeof(char));
			}
			
			else if(*pattern == '.' && !escape_modified(pattern) && *line == *(pattern + 2 * sizeof(char))) {
				init += 2;
				return rgrep_matches(line, pattern + 2 * sizeof(char));
			}
			
			else if(*pattern == '.' && !escape_modified(pattern) && *line != *(pattern + 2 * sizeof(char))) {
				init += 2;
				return rgrep_matches(line + sizeof(char), pattern + 2 * sizeof(char));
			}
			
			else if(*line == *pattern && *line == *(pattern + 2 * sizeof(char))) { // poiting to same char, and line poinet is pointing to same char as the second next char in pattern
				init += 2;
				return rgrep_matches(line, pattern + 2 * sizeof(char));
			}
			
			else if(*line == *pattern && next_ch(line) == *(pattern + 2 * sizeof(char))){ // pointing to same char, and next char in line is same as second next char in pattern
				return 1;
			}
			
			else if(*line == *pattern && next_ch(line) != *(pattern + 2 * sizeof(char)) && *line != *(pattern + 2 * sizeof(char))) // pointing ti same char but the 2 armguments above are not true
				return 0;
			
			else if(*line != *pattern && *line != *(pattern + 2 * sizeof(char))) // poiting to same char , and line pointer value is NOT same as second next char in pattern
				return 0;
			
			else if(*line != *pattern && *line == *(pattern + 2 * sizeof(char))) // poiting to same char , and line pointer value IS same as second next char in pattern
				return 1;
			
			else if(*line == *pattern && *pattern == '.' && escape_modified(pattern)){ // pointing to same char. we have backslash followed by "."
				return 1;
			}
			
			else if(!(*pattern == '.' && !escape_modified(pattern)) || *line != *pattern) // pointing to "." in patterm. we dont have backslash before that, or we are not pointing to "." in line
				line -= sizeof(char);

			pattern += sizeof(char);
			init++;
		}
		pattern += sizeof(char);
		init++;
	}


	else if(init != 0 && *(pattern - init * sizeof(char)) != '\0') { // reset pattern on partial match
		pattern -= init * sizeof(char);
		init -= init;
	}


		line += sizeof(char);

	


	return rgrep_matches(line, pattern);
}
