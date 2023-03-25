using System.Net;
using System.Net.Sockets;
using Models;
public interface ITCPService{
    
    public void start();
    public esp32 getConnection(string name);
    public Dictionary<string, esp32> getConnectionList();
    
}