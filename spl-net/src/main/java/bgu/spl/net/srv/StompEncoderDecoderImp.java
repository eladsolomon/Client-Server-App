package bgu.spl.net.srv;

import bgu.spl.net.api.MessageEncoderDecoder;

import java.nio.charset.StandardCharsets;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Map;

public class StompEncoderDecoderImp implements MessageEncoderDecoder<EladErezFrame> {
    private byte[] bytes = new byte [1<<10];
    private int len=0;

    @Override
    public EladErezFrame decodeNextByte(byte nextByte) {
        if(nextByte == '\0') { //TODO add @ to the condition.
            return popString();
        }
        pushByte(nextByte);
        return null; // didnt finish the frame yet.
    }

    @Override
    public byte[] encode(EladErezFrame frame) {
        return (frame.toString()).getBytes();
    }

    private EladErezFrame popString() {
        String result = new String(bytes, 0, len, StandardCharsets.UTF_8);
        len = 0;
        String lines[] = result.split("\\n");
        String type = lines[0];

        Map<String, String> headers = new HashMap<String, String>();
        int counter=1;
        
        for (;counter< lines.length && !lines[counter].isEmpty(); counter++){
            if(lines[counter].contains(":")){
                String header[] = lines[counter].split(":");
                headers.put(header[0], header[1]);
            }
        }
        counter++;
        String body="";
        while(counter <lines.length && !lines[counter].isEmpty())
        {
            body+=lines[counter];
            counter++;
        }
        EladErezFrame ans = new EladErezFrame(type,headers,body);
        return ans;
    }
    private void pushByte(byte nextByte) {
        if (len >= bytes.length) {
            bytes = Arrays.copyOf(bytes, len * 2);
        }
        bytes[len++] = nextByte;
    }
}
