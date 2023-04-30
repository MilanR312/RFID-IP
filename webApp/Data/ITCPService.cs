using System.Net;
using System.Net.Sockets;
using Models;
public interface ITCPService{
    
    public void start();
    public esp32V2 getConnection(string name);
    public Dictionary<string, esp32V2> getConnectionList();
    
}