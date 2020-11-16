package bgu.spl.net.srv;

import bgu.spl.net.api.StompMessagingProtocol;

public class StompMessagingProtocolImp implements StompMessagingProtocol<EladErezFrame> {
    private int connection_id;
    private boolean shouldTerminate = false;
    private ConnectionsImp connections;

    @Override
    public void start(int connectionId, Connections<EladErezFrame> connections) {
        this.connection_id = connectionId;
        this.connections = (ConnectionsImp)connections;
    }

    @Override
    public void process(EladErezFrame frame) {
            String command=frame.getType();
            switch (command) {
                case "STOMP":
                case "CONNECT":
                    connectAct(frame);
                    break;
                case "SUBSCRIBE":
                    SubscribeAct(frame);
                    break;
                case "UNSUBSCRIBE":
                    unsubscribeAct(frame);
                    break;
                case "DISCONNECT":
                    disconnectAct(frame);
                    break;
                case "SEND":
                    sendAct(frame);
                    break;
        }
    }


    @Override
    public boolean shouldTerminate() {
        return shouldTerminate;
    }

    private void connectAct(EladErezFrame frame) {
         this.connections.connectAct(frame,connection_id);

    }

    private void SubscribeAct(EladErezFrame frame) {
         this.connections.subscribeAct(frame,connection_id);
    }


    private void sendAct(EladErezFrame frame) {
         this.connections.sendAct(frame,connection_id);
    }

    private void unsubscribeAct(EladErezFrame frame) {//exit
         this.connections.unsubscribeAct(frame,connection_id);
    }

    private void disconnectAct(EladErezFrame frame) {
        this.shouldTerminate=true;
        this.connections.disconnectAct(frame,connection_id);

    }

}
