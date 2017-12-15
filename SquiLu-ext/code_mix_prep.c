/* Copyright (c) 2007-2008 Thomas Lavergne
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Adapted by Domingo Alvarez Duarte
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef MixInteger
#define MixInteger int
#endif

#define mix_tempsize 4096

static const char *err_badeof = "unexpected end of file";

enum mix_token_e {
	tok_sh,
	tok_code_start, tok_code_end, tok_expr_start, tok_expr_end,
	tok_data, tok_code, tok_expr,
	tok_eof
};

typedef struct mix_state_s {
	size_t      size;
	const char *buffer;

	const char *code_start;
	size_t      code_startsize;
	const char *code_end;
	size_t      code_endsize;
	const char *expr;
	size_t      exprsize;
	char       print_out[64];
	size_t      print_outsize;

	size_t      pos;
	int         token;
	char        temp[mix_tempsize];

	char        sq_reader_buf[mix_tempsize];
	char        *sq_reader_last_pos;
	char        *sq_reader_pos;

	const char *error;
	MixInteger result_size, result_pos;
	const char *result;
	size_t new_lines;
} mix_state_t;

/* Read an open bracket of the form [=*[ and return the number of = found.
 * if there is no string like this at the current pos return -1.
 * after this call the cursor is set at the first char after the bracket.
 */
static int mix_bracket_open(mix_state_t *S) {
	size_t pos, cnt;
	if (S->buffer[S->pos] != '[')
		return -1;
	pos = S->pos + 1;
	cnt = 0;
	while (pos < S->size && S->buffer[pos] == '=')
		++pos, ++cnt;
	if (pos == S->size || S->buffer[pos] != '[')
		return -1;
	S->pos = pos + 1;
	return cnt;
}

/* Read a close bracket of the form ]=*] and return the number of = found.
 * if there is no string like this at the current pos return -1.
 * after this call the cursor is set at the first char after the bracket.
 */
static int mix_bracket_close(mix_state_t *S) {
	size_t pos, cnt;
	if (S->buffer[S->pos] != ']')
		return -1;
	pos = S->pos + 1;
	cnt = 0;
	while (pos < S->size && S->buffer[pos] == '=')
		++pos, ++cnt;
	if (pos == S->size || S->buffer[pos] != ']')
		return -1;
	S->pos = pos + 1;
	return cnt;
}

/* Skip a lua string enclosed by type (who can be either \" or \') taking
 * account of escape chars. after a call to this the cursor is set at the
 * first character after the string.
 * Beware that on enter the cursor must be on the first char of the string
 * not on the openning char.
 */
static int mix_skip_string(mix_state_t *S, char type) {
	while (S->pos < S->size) {
		if (S->buffer[S->pos] == type) {
			++S->pos;
			return 0;
		}
		if (S->buffer[S->pos] == '\\')
			++S->pos;
		++S->pos;
	}
	return -1;
}

/* Skip a lua long string wich is enclosed by square bracket and level = signs
 * after the call the cursor is set on the first character after the string.
 * Beware that on enter the cursor must be on the first char of the string
 * not on the openning bracket.
 */
static int mix_skip_lstring(mix_state_t *S, int level) {
	while (S->pos < S->size) {
		int tmp = mix_bracket_close(S);
		if (tmp == -1)
			++S->pos;
		else if (tmp == level)
			return 0;
	}
	return -1;
}

/* Skip a squirrel comment either one line or long. after the call the cursor is set
 * to the first character after the coment.
 * Beware that on enter the cursor must be set on the first char after the //
 * comment prefix.
 */
static int mix_skip_comment(mix_state_t *S) {
	int level = mix_bracket_open(S);
	if (level != -1)
		return mix_skip_string(S, level);
	while (S->pos < S->size && S->buffer[S->pos] != '\n')
		++S->pos;
    ++S->new_lines;
    return 0;
}

#define mix_iscode_start(s) (!strncmp((s)->code_start, (s)->buffer + (s)->pos, (s)->code_startsize))
#define mix_iscode_end(s) (!strncmp((s)->code_end, (s)->buffer + (s)->pos, (s)->code_endsize))
#define mix_isexpr(s) (!strncmp((s)->expr, (s)->buffer + (s)->pos, (s)->exprsize))

#define result_literal(s) S->result_size = sizeof(s)-1, S->result = s;

static const char * sq_mix_reader_str(void *ud) {
	mix_state_t *S = (mix_state_t *)ud;
	int retrying_code_start = 0;

try_again:

	if (S->error != NULL)
		return 0;

	/* Data chunk are sent to lua by block cause we must escape some char
	 * and we cannot modify the buffer itself. So we must use another
	 * buffer sending escaped block one by one.
	 * If there is no data to send in the chunk, change the state and go
	 * directly to the tok_code_start part.
	 */
	if (S->token == tok_data) {
		S->result_size = 0;
		while (S->pos < S->size) {
			char c = S->buffer[S->pos];
			if (c == S->code_start[0])
				if (mix_iscode_start(S))
					break;
			if (c == '\\' || c == '"')
				S->temp[S->result_size++] = '\\';
			if (c == '\r' || c == '\n'){
				S->temp[S->result_size++] = '\\';
				if(c == '\r') c = 'r';
				else {
				    S->new_lines++;
				    c = 'n';
				}
			}

			S->temp[S->result_size++] = c;
			++S->pos;
			if (S->result_size >= mix_tempsize - 1) {
				if (S->pos == S->size)
					S->token = tok_code_start;
				return S->result = S->temp;
			}
		}
		if (S->pos < S->size)
			S->pos += S->code_startsize;
		S->token = tok_code_start;
		if (S->result_size != 0)
			return S->result = S->temp;
	}

	/* Send the termination of the string and the function call before
	 * going in tok_code/tok_expr state or if at eof go to tok_eof.
	 */
	if (S->token == tok_code_start) {
		if (S->pos == S->size) {
			S->token = tok_eof;
		} else if (mix_isexpr(S)) {
			S->token = tok_expr_start;
			S->pos += S->exprsize;
		} else {
			S->token = tok_code;
		}
		if(!retrying_code_start) return result_literal("\");");
	}

	/* Send the output function, but without the string start character, so
	 * the result of the lua code will be send as parameters to the output
	 * function.
	 */
	if (S->token == tok_expr_start) {
	    if(S->new_lines){
	        --S->new_lines;
	        return result_literal("\n");
	    }
		S->token = tok_expr;
		S->result_size = S->print_outsize - 1;
		return S->result = S->print_out;
	}

	/* Send a lua chunk in one block to the lua engine. No escaping are
	 * needed here so we can send the full block. Then switch to the
	 * tok_code_end. (this correctly skip all form of lua string and comments)
	 * If no lua code is found, go directly to the tok_code_end part.
	 */
	if (S->token == tok_code || S->token == tok_expr) {
	    if(S->new_lines){
	        --S->new_lines;
	        return result_literal("\n");
	    }
		size_t old = S->pos;
		while (S->pos < S->size) {
			char c = S->buffer[S->pos];
			if (c == '\'') {
				++S->pos;
				mix_skip_string(S, '\'');
			} else if (c == '"') {
				++S->pos;
				mix_skip_string(S, '"');
			} else if (c == '[') {
				int level = mix_bracket_open(S);
				if (level != -1)
					mix_skip_lstring(S, level);
				else
					++S->pos;
			} else if (c == '/') {
				++S->pos;
				if (S->buffer[S->pos] == '/') {
					++S->pos;
					mix_skip_comment(S);
				}
			} else if (c == S->code_end[0]) {
				if (mix_iscode_end(S))
					break;
				++S->pos;
			} else {
				++S->pos;
			}
		}
		S->result_size = S->pos - old;
		if (S->pos == S->size) {
			S->error = err_badeof;
			return NULL;
		}
		S->token = S->token == tok_code ? tok_code_end : tok_expr_end;
		S->pos += S->code_endsize;
		if (S->result_size != 0)
			return S->result = S->buffer + old;
	}

	/* Send the data output function name and (' that start a data block to
	 * the lua engine and switch to the tok_data state.
	 */
	if (S->token == tok_code_end) {
		if(S->new_lines){
			--S->new_lines;
			return result_literal("\n");
		}
		S->token = tok_data;
		if(mix_iscode_start(S))
        {
            //we have an empty str so let's skip it
            retrying_code_start = 1;
            goto try_again;
        }
        if ((S->pos+1) == S->size) return 0;
		S->result_size = S->print_outsize;
		return S->result = S->print_out;
	}

	/* Close the output function call on end of expression and go to the lua
	 * to data transition.
	 */
	if (S->token == tok_expr_end) {
		S->token = tok_code_end;
		return result_literal(");");
	}

	/* If we skipped a sh-bang line we must send an empty line to keep the
	 * lua line counter correct.
	 */
	if (S->token == tok_sh) {
		S->token = tok_code_end;
		++S->new_lines;
		return result_literal("\n");
	}

	return NULL;
}

static MixInteger sq_mix_reader_char(void *ud) {
	mix_state_t *S = (mix_state_t *)ud;

	if (S->error != NULL) return 0;
start:
	if (S->result_size){
	    MixInteger c = S->result[S->result_pos++];
	    if(S->result_pos >= S->result_size){
	        S->result_pos = S->result_size = 0;
	    }
	    return c;
	}
	if(sq_mix_reader_str(ud)) goto start;
	return 0;
}

static void sq_mix_init(mix_state_t *S,
                        char *buffer, int buffer_size,
                        const char *print_out,
                        const char *code_start,
                        const char *code_end,
                        const char *expr){
	memset(S, 0, sizeof(mix_state_t));
	snprintf(S->print_out, sizeof(S->print_out), "%s(\"",  print_out ? print_out : "mix_write");
	S->print_outsize = strlen(S->print_out);
	S->code_start = code_start ? code_start : "{%";
	S->code_startsize = strlen(S->code_start);
	S->code_end = code_end ? code_end : "%}";
	S->code_endsize = strlen(S->code_end);
	S->expr = expr ? expr : "=";
	S->exprsize = strlen(S->expr);
	S->buffer = buffer;
	S->size = buffer_size;
	S->token = tok_code_end;
}

