package bgu.spl.net.srv;

import java.io.IOException;

public interface Connections<T> {

    boolean send(int connectionId, T msg);

    void send(String channel, T msg);
    void addConnect (int i, ConnectionHandler<T> ch);
    void disconnect(int connectionId);
}
