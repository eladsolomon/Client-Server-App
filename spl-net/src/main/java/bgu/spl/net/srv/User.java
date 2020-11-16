package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

public class User{
    private boolean isConnected;
    private String userName;
    private String password;
    private int connection_id;
    private ConcurrentHashMap<String, String> subscriptions_id = new ConcurrentHashMap<>();

    public User(String userName, String password,int connection_id){
        isConnected = false;
        this.userName = userName;
        this.password = password;
        this.connection_id=connection_id;
    }
    public boolean getConnected()
    {
        return this.isConnected;
    }
    public void setConnected()
    {
       isConnected=true;
    }
    public void setDisconnected()
    {
        isConnected=false;
    }
    public int getConnection_id() {
        return connection_id;
    }
    public String getPassword() {
        return password;
    }
    public void setConnection_id(int connection_id) {
        this.connection_id = connection_id;
    }

    public String getUserName() {
        return userName;
    }

    public void clearMap(){
        subscriptions_id.clear();
    }

    public void setSubscriptionId(String topic, String frameID) {
        this.subscriptions_id.put(topic, frameID);
    }

    public ConcurrentHashMap<String, String>  getSubscriptionIdMap() {
        return this.subscriptions_id;
    }

    public String getTopicByName(String topicName) {
        for(String s : subscriptions_id.keySet())
        {
            if(s==topicName)
                return subscriptions_id.get(s);
        }
        return "";
    }
}
