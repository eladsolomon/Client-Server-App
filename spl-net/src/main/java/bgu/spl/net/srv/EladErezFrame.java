package bgu.spl.net.srv;

import java.util.HashMap;
import java.util.Map;

public class EladErezFrame {
    private String type;
    private Map<String,String> headers;
    private String body;

    public EladErezFrame(String type, Map<String,String> headers){
        this.type = type;
        this.headers = headers;
        this.body="";
    }

    public EladErezFrame(String type, Map<String,String> headers, String body){
        this.type = type;
        this.headers = headers;
        this.body = body;
    }

    public Map<String, String> getHeaders() {
        return headers;
    }

    public String getType() {
        return type;
    }
    public void addHeader(String message_index, String subscription_id)
    {
        headers.put("subscription",subscription_id );
        headers.put("message-id", message_index);
    }

    public String getBody() {
        return body;
    }

    @Override
    public String toString() {
        String ans="";
        ans += type + "\n";
        for(String s : headers.keySet()){
            ans += s + ":" + headers.get(s) +"\n";
        }
        ans+="\n";
        ans+=body+"\n";
        ans+='\0';
        return ans;
    }
}
