package anonymous.seclab;

import com.fasterxml.jackson.databind.annotation.JsonSerialize;

@JsonSerialize(using=CounterSerializer.class)
public class Counter {
    public Counter(){
    }
    
    public int getMatched(){
        return HashMatching.matched;
    }
    public int getTried(){
        return HashMatching.tryMatch;
    }
    
}
