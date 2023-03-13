namespace Models;
public class ArtikelRequest{
    public string key {get;set;} = "";
    public string naam {get; set;} = "";
    public int price {get;set;} = 0;
    public string soort {get;set;} = "";
    public string operatorType {get;set;} = "";
    public string operatorType2 {get;set;} = "";

    public int beschikbaar {get;set;}= 0;
}