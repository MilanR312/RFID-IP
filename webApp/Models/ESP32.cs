using System.Net;

public class esp32{
    public string ip {get; set;}
    public int port {get; set;}
    public string name {get; set;}
    public string? location {get; set;}
    
    public esp32(string addres, int port, string name){
        this.ip = addres;
        this.port = port;
        this.name = name;
    }
    public esp32(){
        this.ip = "0.0.0.0";
        this.port = 0;
        this.name =" temp";
    }
}