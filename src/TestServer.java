import java.io.*;
import java.net.*;
import java.nio.*;

class TestServer
{
   public static void main(String argv[]) throws Exception
      {
         ServerSocket welcomeSocket = new ServerSocket(6789);

         while(true)
         {
            Socket connectionSocket = welcomeSocket.accept();
            System.out.println("welcomeSocket.accept() called");
            DataInputStream inFromClient = new DataInputStream(connectionSocket.getInputStream());
            DataOutputStream outToClient = new DataOutputStream(connectionSocket.getOutputStream());


            System.out.println("about to read int");
            
            int result = inFromClient.readInt();

            System.out.println("result: " + result);
            String randomString = "asdfWE872FWed";
            result++;

            outToClient.writeInt(result);
            outToClient.writeChars(randomString);


            System.out.println("string written");

            BufferedReader d = new BufferedReader(new InputStreamReader(connectionSocket.getInputStream()));
            String bla = d.readLine();

            if(bla.equals("ASDFwe872fwED")){
               System.out.println("OK");
               // outToClient.writeChars("OK");
            } else {
               System.out.println("FAIL");
               // outToClient.writeChars("FAIL");
            }

         }
      }
}
