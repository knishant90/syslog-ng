from __future__ import print_function, absolute_import
from .lexertoken import Token
from .lexer import Lexer

CLL_NORMAL = 0
CLL_QUOTED_STRING = 1
CLL_QUOTED_CHAR = 2


class CommandLineLexer(Lexer):
    """This an inperfect lexer for both the debug language and template functions"""
    def __init__(self):
        self._tokens = []
        self._current_token = 0

    def input(self, input):
        self._input = input
        self._current_position = 0
        self._current_state = CLL_NORMAL
        self._paren_balance = 0

    def token(self):
        token = self._get_next_token()
        return token

    def get_position(self):
        return self._current_position

    def _get_next_token(self):
        self._skip_whitespace()
        self._current_token = ''
        self._paren_balance = 0
        start_position = self._current_position
        token = None

        while self._current_position < len(self._input):
            current_char = self._input[self._current_position]

            if self._current_state == CLL_NORMAL:
                token = self._process_normal_character(current_char)
            elif self._current_state == CLL_QUOTED_STRING:
                token = self._process_string_character(current_char)
            elif self._current_state == CLL_QUOTED_CHAR:
                token = self._process_escaped_character(current_char)

            self._current_position += 1
            if token is not None:
                return token

        if start_position != self._current_position:
            partial = False
            if self._current_state != CLL_NORMAL:
                partial = True
            if self._paren_balance != 0:
                partial = True
            return Token('ARG', value=self._current_token, partial=partial)

    def _skip_whitespace(self):
        while self._current_position < len(self._input) and self._input[self._current_position].isspace():
            self._current_position += 1

    def _process_normal_character(self, current_char):
        if current_char == '"' or current_char == "'":
            return self._open_quoted_string(current_char)
        elif current_char.isspace():
            return self._close_current_token(current_char)
        elif current_char == '(':
            return self._open_paren(current_char)
        elif current_char == ')':
            return self._close_paren(current_char)
        else:
            self._current_token += current_char

    def _open_paren(self, current_char):
        self._paren_balance += 1
        self._current_token += current_char

    def _close_paren(self, current_char):
        self._paren_balance -= 1
        if self._paren_balance <= 0:
            self._current_token += current_char
            return Token('ARG', value=self._current_token)
        self._current_token += current_char

    def _close_current_token(self, current_char):
        if self._paren_balance == 0:
            return Token('ARG', value=self._current_token)
        self._current_token += current_char

    def _process_string_character(self, current_char):
        if current_char == '\\':
            self._current_state = CLL_QUOTED_CHAR
        elif current_char == self._quote_open_character:
            self._close_quoted_string(current_char)
        else:
            self._current_token += current_char

    def _process_escaped_character(self, current_char):
        self._current_token += current_char
        self._current_state = CLL_QUOTED_STRING

    def _open_quoted_string(self, current_char):
        self._quote_open_character = current_char
        self._current_state = CLL_QUOTED_STRING
        if self._paren_balance > 0:
            self._current_token += current_char

    def _close_quoted_string(self, current_char):
        if self._paren_balance > 0:
            self._current_token += current_char
        self._current_state = CLL_NORMAL