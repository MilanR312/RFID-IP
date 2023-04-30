using Models;
using System.Net.Sockets;
using System.Text;
using System.Threading.Channels;
public class esp32{

    private TcpClient client;
    private NetworkStream tcpStream;
    private Channel<esp32request> ddataToSend = Channel.CreateUnbounded<esp32request>();
    
    private callbackQueue<esp32request> dataToSend = new();
    //private Task sendTask;


    public string name{get;set;} = "";

    public string location{get;set;} = "";
    public string latestLoging{get;set;} = "";
    public int[] buttons = new int[2]; //-1 invalid, 0: off, 1: on

    public bool disconnected = false;
    public esp32(TcpClient client){
        this.client = client;
        tcpStream = client.GetStream();

        Task.Run(async () => {
            while(true){
                Console.WriteLine("waiting for data");
                var result = await dataToSend.readBlocking();
                
                esp32request data = result.Item1;
                Console.WriteLine($"sending {data.command} to esp32");
                await send(data.command);
                
                if (data.expectReturn){
                    string r = await Receive();
                    Console.WriteLine($"received {r}");
                    result.Item2(r);
                }
                result.Item3.Cancel();
            }
        });
        
    }


    public async Task updateData(){
        if (disconnected) return;
        if (this.name == ""){
            await dataToSend.add(new(true, "GETNAME\n"), v => {
                this.name = v;
            });
        }
        if (this.location == ""){
            await dataToSend.add(new(true, "GETLOCATION\n"), v => {
                this.location = v;
            });
        }
        for (int i = 0; i < this.buttons.Length ; i++){
            if (buttons[i] == -1){
                await dataToSend.add(new(true, $"GETBUTTON{i+1}\n"), v => {
                    buttons[i] = int.Parse(v);
                });
            }
        }
        if (this.latestLoging == ""){
            await dataToSend.add(new(true, "GETLLOGIN\n"), v => {
                this.latestLoging = v;
            });
        }
        
    }

    public async Task<int> toggleButton(int button){
        if (disconnected) return -1;
        int oldval = this.buttons[button-1];
        int newVal = (oldval == 0) ? 1 : 0;
        disconnected = await dataToSend.add(new(false, $"SETBUTTON{button}{newVal}\n"), v => {});
        disconnected = await dataToSend.add(new(true, $"GETBUTTON{button}\n"), v =>{
            this.buttons[button-1] = int.Parse(v);
            Console.WriteLine($"got value {int.Parse(v)}");
        });
        return buttons[button-1];
    }

    private async Task<string> Receive(){
        byte[] buffer = new byte[32];
        await tcpStream.ReadAsync(buffer,0,32);
        return Encoding.ASCII.GetString(buffer);
    }
    private async Task send(string message){
        if (message.Length > 32) throw new Exception("too long of a message");
        //send ths tring as a c style string
        byte[] data = Encoding.ASCII.GetBytes(message.PadRight(33,'\0'));
        await tcpStream.WriteAsync(data,0,data.Length);
    }
}