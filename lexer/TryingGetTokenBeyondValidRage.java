package com.abc.pkg.basics.lexer;

import com.abc.pkg.alg.Trie.Tr;

/**
 * Created by c7s on 2016/1/21.
 */
public class TryingGetTokenBeyondValidRage extends RuntimeException{
    public TryingGetTokenBeyondValidRage(String msg) {
        super(msg);
    }

    public TryingGetTokenBeyondValidRage() {
        super("try get next token when current is EOF of token");
    }
}
