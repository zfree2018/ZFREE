package anonymous.seclab;

import java.io.IOException;


import com.fasterxml.jackson.core.JsonGenerator;
import com.fasterxml.jackson.core.JsonGenerator.Feature;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.JsonSerializer;
import com.fasterxml.jackson.databind.SerializerProvider;

public class CounterSerializer extends JsonSerializer<Counter>{

    @Override
    public void serialize(Counter counter, JsonGenerator jGen, SerializerProvider serializer)
            throws IOException, JsonProcessingException {
        jGen.configure(Feature.WRITE_NUMBERS_AS_STRINGS, true);
        
        if(counter == null){
            jGen.writeStartObject();
            jGen.writeFieldName("err");
            jGen.writeString("No counter avaliable");
            jGen.writeEndObject();
            return;
        }
        jGen.writeStartObject();
        jGen.writeFieldName("matched");
        jGen.writeString(Integer.toString(counter.getMatched()));
        jGen.writeFieldName("tried");
        jGen.writeString(Integer.toString(counter.getTried()));
        jGen.writeEndObject();
    }

}
