package com.abc.pkg.basics.lexer;

/**
 * Created by c7s on 2016/1/21.
 */
public class NaiveLexer implements Lexer {
    private String input;
    private char c;
    private int p;
    private final char EOF = (char) -1;

    public NaiveLexer(String input) {
        initLexer(input);
    }

    @Override
    public void resetLexer(String inputParam) {
        initLexer(inputParam);
    }

    private void initLexer(String newInput) {
        checkInput(newInput);
        this.input = newInput;
        c = input.charAt(0);
        p = 0;
    }

    private void checkInput(String str) {
        if (str.length() == 0) {
            throw new EmptyInputStringException(String.format("input: %s", str));
        }
    }

    private void consume() {
        p++;
        if (p == input.length()) {
            c = EOF;
            return;
        } else if (p > input.length()) {
            throw new ExceedEndOfInputString("reach the end of input string");
        }
        c = input.charAt(p);
    }

    private void appendAndConsume(StringBuilder builder) {
        builder.append(c);
        consume();
    }

    private StringBuilder consumeContinuousNumbers() {
        StringBuilder builder = new StringBuilder();
        while (c != EOF) {
            if (c >= '0' && c <= '9') {
                builder.append(c);
            } else {
                break;
            }
            consume();
        }
        return builder;
    }

    private String getNumber() {
        StringBuilder builder = new StringBuilder();
        //possibly begin with '-'
        if (c == '-') {
            appendAndConsume(builder);
        }
        //if it's 0, 0 must preceed a '.', otherwise it's 1-9. If so, any number of 0-1 may follow
        if (!Character.isDigit(c)) {
            throw new NotValidNumber("no digit follows '-' or not begin with digit when it supposes to be number");
        }
        if (c == 0) {
            appendAndConsume(builder);
        } else {
            builder.append(consumeContinuousNumbers());
        }
        //possibly fractions here. see if '.' exists
        if (c == '.') {
            appendAndConsume(builder);
            builder.append(consumeContinuousNumbers());
        }
        //take care of exponents
        if (c == 'e' || c == 'E') {
            appendAndConsume(builder);
            //'-', '+' may proceed numbers
            if (c == '-' || c == '+') {
                appendAndConsume(builder);
            }
            builder.append(consumeContinuousNumbers());
        }

        return builder.toString();
    }

    private char peek(int pos) {
        if (pos < input.length()) {
            return input.charAt(pos);
        } else if (pos == input.length()) {
            return EOF;
        } else {
            throw new ExceedEndOfInputString("peek");
        }
    }

    private boolean extractEscapedCharacter(StringBuilder builder) {
        StringBuilder local = new StringBuilder();
        char first = c;
        char second = peek(p + 1);
        if (first == '\\') {
            local.append(first);
            if (second == 't' || second == '\\' || second == 'r' || second == '"'
                    || second == '/' || second == 'b' || second == 'f' || second == 'n') {
                local.append(second);

                builder.append(local);
                //update member variable c and p
                consume();
                consume();
                return true;
            } else if (second == 'u') {
                local.append(second);

                //there must be 4 hex digits after '\ u'
                if (p + 6 > input.length()) {
                    //no enough char to hold the \ uxxxx
                    throw new NotValidStringLiteral("cannot get enough chars for unicode '\' 'u' and 4 hex digits before drain input ");
                }
                int i = 2;
                for (; i < 6; i++) {
                    char tmp = peek(p + i);
                    if (Character.isDigit(tmp) || tmp >= 'a' && tmp <= 'f' || tmp >= 'A' && tmp <= 'F') {
                        local.append(tmp);
                    } else {
                        break;
                    }
                }
                if (i != 6) {
                    //less than 4 hex digits exist here
                    throw new NotValidStringLiteral("less than 4 hex digits exist after '\' and 'u' ");
                } else {
                    builder.append(local);
                    //update member variable c and p
                    for (int j = 0; j < 6; j++) {
                        consume();
                    }
                    return true;
                }
            }
        }
        return false;
    }

    private String getStringLiteral() {
        StringBuilder builder = new StringBuilder();
        while (c != EOF) {
            if (Character.isLetter(c)) {
                builder.append(c);
            } else if (extractEscapedCharacter(builder)) {
                //in ex.. we've already went over valid escaped chars and updated c and p
                continue;
            } else {
                break;
            }
            consume();
        }
        return builder.toString();
    }

    private void ignoreWhiteSpace() {
        while (c != EOF) {
            if (Character.isWhitespace(c)) {
                consume();
            } else {
                break;
            }
        }
    }

    @Override
    public Token nextToken() {
        while (c != EOF) {
            switch (c) {
                case '{':
                    consume();
                    return new Token(TokenType.LBRACE);
                case '}':
                    consume();
                    return new Token(TokenType.RBRACE);
                case '[':
                    consume();
                    return new Token(TokenType.LBRACKET);
                case ']':
                    consume();
                    return new Token(TokenType.RBRACKET);
                case ',':
                    consume();
                    return new Token(TokenType.COMMA);
                case '"':
                    consume();
                    return new Token(TokenType.DOUBLEQUOTE);
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    ignoreWhiteSpace();
                    break;
                case ':':
                    consume();
                    return new Token(TokenType.COLON);
                default:
                    if (Character.isDigit(c) || c == '-') {
                        String number = getNumber();
                        return new Token(TokenType.NUMBER, number);
                    }
                    String literal = getStringLiteral();
                    return new Token(TokenType.STRINGLITERAL, literal);
            }
        }
        return new Token(TokenType.NONE);
    }

    public static void main(String[] args) {
        String input = "[a,b,\\t,\\n,\\f,\\r,\\u1234]][{}:\\/,L\\u12aF,A,B,  ,1,-1,123,123.123E+123, -0.1234e-123,123.123E123";
        Lexer naiveLexer = new NaiveLexer(input);
        Token t = naiveLexer.nextToken();
        while (t.getType() != TokenType.NONE) {
            System.out.println(t);
            t = naiveLexer.nextToken();
        }
    }
}
