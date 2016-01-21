package com.abc.pkg.basics.lexer;

import static com.abc.pkg.basics.lexer.TokenType.*;

/**
 * Created by c7s on 2016/1/21.
 */
public class NaiveParser {
    private Token token;
    private Lexer lexer;

    public NaiveParser(String s) {
        initParser(s);
    }

    public void initParser(String input) {
        lexer = new NaiveLexer(input);
        token = lexer.nextToken();
    }

    public void consume() {
        if (token.getType() == TokenType.NONE) {
            throw new TryingGetTokenBeyondValidRage();
        }
        token = lexer.nextToken();
        System.out.println("token = " + token);
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

    public boolean matchElements() {
        while (matchValue()) {
            if (token.getType() != COMMA) {
                return true;
            }
            consume();
        }
        return false;
    }

    public boolean matchArray() {
        if (!match(LBRACKET)) {
            return false;
        }
        if (token.getType() != RBRACKET) {
            if (!matchElements()) {
                return false;
            }
        }
        return match(RBRACKET);
    }

    public boolean matchValue() {
        switch (token.getType()) {
            case DOUBLEQUOTE:
                return matchStringWithQuote();
            case LBRACKET:
                return matchArray();
            case LBRACE:
                return matchObject();
            case NUMBER:
                return match(NUMBER);
            default:
                if (match("true") || match("false") || match("null")) {
                    return true;
                }
        }
        return  false;
    }

    public boolean matchStringWithQuote() {
        if (!match(DOUBLEQUOTE)) {
            return false;
        }
        if (token.getType() != DOUBLEQUOTE) {
            if (!match(STRINGLITERAL)) {
                return false;
            }
        }
        return match(DOUBLEQUOTE);
    }

    public boolean matchPair() {
        return matchStringWithQuote() && match(COLON) && matchValue();
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
        if (!match(LBRACE)) {
            return false;
        }
        if (token.getType() != RBRACE) {
            if (!matchMembers()) {
                return  false;
            }
        }
        return match(RBRACE);
    }

    public boolean checker() {
        switch (token.getType()) {
            case LBRACE:
                return matchObject();
            case LBRACKET:
                return matchArray();
            case DOUBLEQUOTE:
                return matchValue();
            default:
                if (matchValue()) {
                    return true;
                }
                System.out.println("not object, array nor value");
                return false;
        }
    }

    public void test(String str) {
        initParser(str);
        System.out.println("test begin with token: " + token);
        System.out.println(checker());
    }

    public static void main(String[] args) {
        NaiveParser naiveParser = new NaiveParser("need a lexer factory?");
        naiveParser.test("{}");
        naiveParser.test("[]");
        naiveParser.test("true");
        naiveParser.test("false");
        naiveParser.test("null");
        naiveParser.test("{\"key\" : \"value\", \"num\":123, \"array\" : [1,2,3,4,5]}");

    }
}
