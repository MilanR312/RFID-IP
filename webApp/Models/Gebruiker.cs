namespace Models{
    public class Gebruiker{
        public int id {get;set;}
        public string naam {get;set;} = "";
        public int leeftijd {get;set;}
        public string adres {get;set;} = "";
        public string telefoonNummer {get;set;} = "";
        public string functie {get;set;} = "";

        public Gebruiker(int id, string naam, int leeftijd, string adres, string tel, string functie){
            this.id = id;
            this.naam = naam;
            this.leeftijd = leeftijd;
            this.adres = adres;
            this.telefoonNummer = tel;
            this.functie = functie;
        }
        public Gebruiker(){

        }

    }
}

