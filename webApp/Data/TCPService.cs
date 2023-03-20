using Models;
using System.Net;
using System.Net.Sockets;
using System.Data.Common;
using System.Text;
public class TCPService : ITCPService{
    private TcpListener listener;
    
    private Mutex mapLock = new();
    private Dictionary<string, TcpClient> connections = new();

    private Task tcpTask;
    public TCPService(TcpListener listener){
        this.listener = listener;
        tcpTask = new(async () => {
            while (true){
                Console.WriteLine("starting connection");
                var connection = await listener.AcceptTcpClientAsync();
                using var tcpStream = connection.GetStream();
                string command = "GET\n";
                byte[] data = Encoding.ASCII.GetBytes(command);
                await tcpStream.WriteAsync(data,0,data.Length);
                Console.WriteLine("sending");
                byte[] buffer = new byte[32]; //name max length is 32
                await tcpStream.ReadAsync(buffer,0,32);
                string name = Encoding.ASCII.GetString(buffer);
                //test connection and get name
                mapLock.WaitOne();
                connections.Add(name, connection);
                Console.WriteLine($"added a connection={name}");
                mapLock.ReleaseMutex();
            }
        });
    }

    public void start(){
        tcpTask.Start();
    }
    public TcpClient getConnection(string name){
        return connections[name];
    }
    public Dictionary<string, TcpClient> getConnectionList(){
        return connections;
    }
}