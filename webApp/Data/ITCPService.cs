using System.Net;
using System.Net.Sockets;
using Models;
public interface ITCPService{
    
    public void start();
    public TcpClient getConnection(string name);
    public Dictionary<string, TcpClient> getConnectionList();
    
}