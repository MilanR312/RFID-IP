using Models;
using System.Net;
using System.Net.Sockets;
using System.Data.Common;
using System.Text;
public class TCPService : ITCPService{
    private TcpListener listener;
    
    private Mutex mapLock = new();
    private Dictionary<string, esp32> connections = new();

    private Task tcpTask;
    public TCPService(TcpListener listener){
        this.listener = listener;
        tcpTask = new(async () => {
            while (true){
                Console.WriteLine("starting connection");
                var connection = await listener.AcceptTcpClientAsync();
                var tcpStream = connection.GetStream();
                string command = "GETNAME\n";
                byte[] data = Encoding.ASCII.GetBytes(command);
                await tcpStream.WriteAsync(data,0,data.Length);
                Console.WriteLine("sending");
                byte[] buffer = new byte[32]; //name max length is 32
                await tcpStream.ReadAsync(buffer,0,32);
                string name = Encoding.ASCII.GetString(buffer);
                //test connection and get name
                mapLock.WaitOne();
                if (connections.ContainsKey(name))
                    connections.Remove(name);
                connections.Add(name, new(connection));
                Console.WriteLine($"added a connection={name}");
                mapLock.ReleaseMutex();
            }
        });
    }

    public void start(){
        tcpTask.Start();
    }
    public esp32 getConnection(string name){
        return connections[name];
    }
    public Dictionary<string, esp32> getConnectionList(){
        cull();
        return connections;
    }
    public void cull(){
        /*foreach(var item in connections){
            if (!item.Value.client.Connected){
                item.Value.client.Close();
                Console.WriteLine($"culling {item.Key}");
                connections.Remove(item.Key);
            }
        }*/
    }
}