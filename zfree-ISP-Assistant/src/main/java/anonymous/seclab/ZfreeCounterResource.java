package anonymous.seclab;

import org.restlet.resource.Get;
import org.restlet.resource.ServerResource;

public class ZfreeCounterResource extends ServerResource{
    
    @Get("json")
    public Counter getMatchedCount(){
        return new Counter();
    }
}
