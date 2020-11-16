package bgu.spl.net.impl.stomp;

import bgu.spl.net.api.MessageEncoderDecoder;
import bgu.spl.net.api.MessagingProtocol;
import bgu.spl.net.api.StompMessagingProtocol;
import bgu.spl.net.srv.*;

import java.io.Serializable;
import java.util.function.Supplier;

public class StompServer {

    public static void main(String[] args) {

        int port = Integer.parseInt(args[0]);
        if (args[1].equals("tpc")) {
            Server.threadPerClient(
                    port,
                    StompMessagingProtocolImp::new, //protocol factory
                    StompEncoderDecoderImp::new,//message encoder decoder factory
                    new ConnectionsImp()
            ).serve();
        } else if (args[1].equals("reactor")) {
            Server.reactor(
                    4,
                    port,
                    () -> new StompMessagingProtocolImp(), //protocol factory
                    () -> new StompEncoderDecoderImp(), new ConnectionsImp() //message encoder decoder factory
            ).serve();

        }
    }
}


