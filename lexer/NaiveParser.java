package com.abc.pkg.basics.lexer;

import static com.abc.pkg.basics.lexer.TokenType.*;

/**
 * Created by c7s on 2016/1/21.
 */
public class NaiveParser {
    private Token token;
    private Lexer lexer;

    public NaiveParser(String input) {
        lexer = new NaiveLexer(input);
        token = lexer.nextToken();
    }

    public void consume() {
        if (token.getType() == TokenType.NONE) {
            throw new TryingGetTokenBeyondValidRage();
        }
        token = lexer.nextToken();
    }

    public boolean match(TokenType type) {
        if (token.getType() == type) {
            consume();
            return true;
        }
        return false;
    }

    public boolean match(String str) {
        if (token.getType() == STRINGLITERAL && token.getContent().equals(str)) {
            consume();
            return true;
        }
        return false;
    }

    public boolean matchArray() {
        return match(LBRACKET) && matchValue() && match(RBRACKET);
    }

    public boolean matchValue() {
        return match(STRINGLITERAL) || match(NUMBER) || matchObject() || matchArray()
                || match("true") || match("false") || match("null");
    }

    public boolean matchPair() {
        return match(DOUBLEQUOTE) &&
                match(STRINGLITERAL) &&
                match(DOUBLEQUOTE) &&
                match(COLON) &&
                matchValue();
    }

    public boolean matchMembers() {
        while (matchPair()) {
            if (token.getType() != COMMA) {
                return true;
            }
            consume();
        }
        return false;
    }

    public boolean matchObject() {
        return match(TokenType.LBRACE) &&
            matchMembers() &&
            match(TokenType.RBRACE);
    }

    public void test(String str) {
        lexer.resetLexer(str);

    }
    public static void main(String[] args) {
        String input = "{}";
    }
}
