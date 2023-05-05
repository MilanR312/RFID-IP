using Models;
using System.Net.Sockets;
using System.Text;
using System.Threading.Channels;

public class esp32V2{
    private TcpClient client;
    private NetworkStream tcpStream;

    //0001 1 000 xxxx .... max 32 bytes or 29 bytes for packet
    // ^   ^      ^
    // |   |       data packet
    // |   |  
    // |   read = 0, write = 1
    // state enum
    //private Dictionary<UInt16, Task> transactions = new();
    private Dictionary<state, Action<string>> defaultCallbacks = new();

    public string name {get;set;} = "";
    public string location {get;set;} = "";
    public string latestLogin {get;set;} = "";
    public int[] buttons = new int[2];
    public bool disconnected = false;

    public Action? stateHasChanged;

    public string status;
    public List<string> statusbutton;

    bool multiCall = false;

    public esp32V2(TcpClient client){
        status = "";
        statusbutton = new();
        this.client = client;
        tcpStream = client.GetStream();

        registerCallback(state.NAME, result => {
            this.name = result;
            if (stateHasChanged != null) stateHasChanged();
        });
        registerCallback(state.LOCATION, result => {
            this.location = result;
            if (stateHasChanged != null) stateHasChanged();
        });
        registerCallback(state.LOGIN, result => {
            this.latestLogin = result;
            if (stateHasChanged != null) stateHasChanged();
        });
        registerCallback(state.BUTTON, result => {
            //read data bit
            var resultArr = result.ToCharArray();
            Console.WriteLine(resultArr);
            int index = resultArr[0] - '0';
            int val = resultArr[1] - '0';
            this.buttons[index] = val;
            if (stateHasChanged != null) stateHasChanged();
        });

        Task.Run(async () => {
            while (true){
                string result = await Receive();
                char[] arr = result.ToCharArray();
                state x = (state)(arr[0]-'0');
                string data = result.Substring(1);
                Console.WriteLine("received = " + result);
                Console.WriteLine(x);
                defaultCallbacks[x](data);
            }
        });
        
    }
    public async Task updateData(){
        await send(state.NAME);
        await send(state.LOCATION);
        await send(state.LOGIN);
        for (int i = 0; i < this.buttons.Length; i++){
            await send(state.BUTTON, $"{i}");
        }
    }
    public async Task<int> toggleButton(int button){
        if (disconnected) return -1;
        int oldval = this.buttons[button-1];
        int newVal = (oldval == 0) ? 1 : 0;
        await send(state.BUTTON, $"{button-1}{newVal}", true);
        return buttons[button-1];
    }
    private async Task<string> Receive(){
        byte[] buffer = new byte[32];
        Console.WriteLine("wating for data");
        //await tcpStream.ReadAsync(buffer,0,32);
        await tcpStream.ReadExactlyAsync(buffer, 0, 31);
        return Encoding.ASCII.GetString(buffer);
    }
    private void registerCallback(state key, Action<string> callback){
        defaultCallbacks.Add(key, callback);
    }
    
    private async Task send(state val, string message="", bool write = false){
        if (message.Length > 29) throw new Exception("too long of a message");
        
        
        string toSend = $"{(int)val}{(write ? 1: 0)}" + message + "\n";

        Console.WriteLine(" sending " + toSend);

        //send ths tring as a c style string
        byte[] data = Encoding.ASCII.GetBytes(toSend.PadRight(33,'\0'));
        await tcpStream.WriteAsync(data,0,data.Length);
        //transactions.Add(uniqueKey, t);
    }



    private static Random rn = new();

}