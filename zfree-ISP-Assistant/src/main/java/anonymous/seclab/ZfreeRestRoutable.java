package anonymous.seclab;

import org.restlet.Context;
import org.restlet.Restlet;
import org.restlet.routing.Router;

import net.floodlightcontroller.restserver.RestletRoutable;

public class ZfreeRestRoutable implements RestletRoutable{

    @Override
    public Restlet getRestlet(Context context) {
        Router router = new Router();
        router.attach("/matched", ZfreeCounterResource.class);
        return router;
    }

    @Override
    public String basePath() {
        
        return "/zfree";
    }

}
