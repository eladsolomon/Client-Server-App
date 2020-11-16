package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.*;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import java.util.concurrent.locks.ReentrantLock;

public class ConnectionsImp implements Connections<EladErezFrame> {
    private ConcurrentHashMap <Integer, ConnectionHandler<EladErezFrame>> activeClients = new ConcurrentHashMap<>();
   // private ConcurrentHashMap <String,String> username_password = new ConcurrentHashMap<>();
    private ConcurrentHashMap <String,User> username_User = new ConcurrentHashMap<>();
    private ConcurrentHashMap <String, Queue<User>> topics = new ConcurrentHashMap<>();
    private AtomicInteger messageIndex = new AtomicInteger(0);
    private ConcurrentHashMap<String , ReentrantLock> lockerMap = new ConcurrentHashMap<>();

    public void addConnect(int connectionId, ConnectionHandler<EladErezFrame> client){
        activeClients.put(connectionId,client);
    }

    @Override
    public boolean send(int connectionId, EladErezFrame msg) {
        if(!activeClients.containsKey(connectionId))
            return false;
        else
        {
            for(String s : username_User.keySet())
                if(username_User.get(s).getConnection_id() == connectionId) {
                    lockerMap.get(s).lock();
                    activeClients.get(connectionId).send(msg);
                    lockerMap.get(s).unlock();
                }
          return true;
        }
    }
    @Override
    public void send(String channel, EladErezFrame msg) {
        if(topics.containsKey(channel)) {
                for (User user : topics.get(channel)) {
                    String subscriptionId =user.getSubscriptionIdMap().get(channel);
                    msg.addHeader(Integer.toString(messageIndex.getAndIncrement()), subscriptionId);
                    lockerMap.get(user.getUserName()).lock();
                    if(activeClients.containsKey(user.getConnection_id()))

                    activeClients.get(user.getConnection_id()).send(msg);
                    lockerMap.get(user.getUserName()).unlock();
                }
            }
        }


    @Override
    public void disconnect(int connectionId) {
            for(String chanel : topics.keySet()){
                for(User u : topics.get(chanel)){
                    if(u.getConnection_id() == connectionId) {

                        lockerMap.get(u.getUserName()).lock();
                        topics.get(chanel).remove(u);

                        lockerMap.get(u.getUserName()).unlock();
                    }
                }
            }

        }
    public void connectAct(EladErezFrame frame, int connectionId) {
            String username=frame.getHeaders().get("login");
            String password =frame.getHeaders().get("passcode");
            //String version =frame.getHeaders().get("version");
            if(username_User.containsKey(username)) {
                if (username_User.get(username).getPassword().equals(password)) {
                    if(!username_User.get(username).getConnected()) {
                        username_User.get(username).setConnected();
                        username_User.get(username).setConnection_id(connectionId);//user connect
                        HashMap<String, String> header = new HashMap();
                        header.put("version", "1.2");
                        send(connectionId, new EladErezFrame("CONNECTED", header));
                    }
                    else{
                        HashMap<String, String> header =  new HashMap();
                        header.put("receipt-id",frame.getHeaders().get("receipt"));
                        header.put("message","User already logged in");
                        username_User.get(username).setConnection_id(connectionId);
                        send (connectionId,new EladErezFrame("ERROR", header));
                    }
                }
                else//password wrong
                {
                    HashMap<String, String> header =  new HashMap();
                    header.put("receipt-id",frame.getHeaders().get("receipt"));
                    header.put("message","Wrong Password");
                    username_User.get(username).setConnection_id(connectionId);
                    send (connectionId,new EladErezFrame("ERROR", header));
                }
            }
            else
            {
                User u = new User (username,password,connectionId);
                username_User.put(username,u);
                username_User.get((username)).setConnected();
                lockerMap.put(username,new ReentrantLock());
                HashMap<String, String> header =  new HashMap();
                header.put("version","1.2");
                send (connectionId,new EladErezFrame("CONNECTED", header));
            }
    }

    public void subscribeAct(EladErezFrame frame, int connectionId) {
        String genre = frame.getHeaders().get("destination");

        for (String s : username_User.keySet()) {
            if (username_User.get(s).getConnection_id() == connectionId) {
                if (username_User.get(s).getConnected()) {
                        if(!(topics.containsKey(genre)))
                    {
                        ConcurrentLinkedQueue<User> li = new ConcurrentLinkedQueue<>();
                        li.add(username_User.get(s));
                        topics.put(genre,li);
                    }
                    else
                        topics.get(genre).add(username_User.get(s));
                        String frameID = frame.getHeaders().get("id");
                        String topic = frame.getHeaders().get("destination");
                         username_User.get(s).setSubscriptionId(topic, frameID);
                    if(frame.getHeaders().containsKey("receipt")) {
                        String receiptId = frame.getHeaders().get("receipt");
                        HashMap<String, String> header = new HashMap();
                        header.put("receipt-id", receiptId);
                        send(connectionId, new EladErezFrame("RECEIPT", header));
                    }
                }
            }
        }
    }

    public void disconnectAct(EladErezFrame frame, int connectionId) {
        for (String s : username_User.keySet()) {
            if (username_User.get(s).getConnection_id() == connectionId) {
                if (username_User.get(s).getConnected()) {
                    HashMap<String, String> header = new HashMap();
                    if (frame.getHeaders().containsKey("receipt")){
                        header.put("receipt-id", frame.getHeaders().get("receipt"));
                        send (connectionId, new EladErezFrame("RECEIPT", header));
                    }
                    username_User.get(s).setDisconnected();
                    username_User.get(s).clearMap();
                    lockerMap.get(s).lock();
                    activeClients.remove(connectionId);
                    lockerMap.get(s).unlock();
                    disconnect(connectionId);
                }
            }
        }
    }

    public void unsubscribeAct(EladErezFrame frame, int connectionId) {
        String topic="";
        for (String user : username_User.keySet()) {
            if (username_User.get(user).getConnection_id() == connectionId) {
                if (username_User.get(user).getConnected()) {
                    String subscriptionId = frame.getHeaders().get("id");
                    for (String topicName : username_User.get(user).getSubscriptionIdMap().keySet()) {
                        if (username_User.get(user).getTopicByName(topicName).equals(subscriptionId))
                            topic = topicName;
                    }
                    for (String topicName : topics.keySet()) {
                        if (topicName.equals(topic)) {
                            for (User us : topics.get(topic)) {
                                if (us == username_User.get(user)) {
                                    topics.get(topic).remove(username_User.get(user));
                                    HashMap<String, String> header = new HashMap();
                                    if (frame.getHeaders().containsKey("receipt")) {
                                        header.put("receipt-id", frame.getHeaders().get("receipt"));//TODO CHECK
                                        send(connectionId, new EladErezFrame("RECEIPT", header));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    public void sendAct(EladErezFrame frame, int connectionId) {
        for (String s : username_User.keySet()) {
            if (username_User.get(s).getConnection_id() == connectionId) {
                if (username_User.get(s).getConnected()) {
                    HashMap<String, String> header = new HashMap();
                    //subscriptionId and messageIndex will added privatly for each frame in the next send function.
                    header.put("destination",frame.getHeaders().get("destination"));
                    String body = frame.getBody() + "\n";
                    send (frame.getHeaders().get("destination"), new EladErezFrame("MESSAGE", header, body));
                }
            }
        }
    }
}

