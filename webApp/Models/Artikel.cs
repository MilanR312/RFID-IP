

namespace Models{
    public class Artikel{
        static readonly string allowedChars = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
        public string code {get; set;}
        public string? naam {get; set;}
        public int? beschikbaar {get; set;}
        public int? prijs {get; set;}
        public string? soort {get; set;}

        Artikel(){
            Random rng = new();
            char[] chars = new char[16];
            for (int i = 0; i < 16; i++){
                chars[i] = allowedChars[rng.Next(allowedChars.Length)];
            }
            code = new String(chars, 0, 16);
        }
    }
}
