using Models;
public interface IGebruikerService{
    Task<bool> CreateGebruiker(Gebruiker gebruiker);
    Task<List<Gebruiker>> getGebruikerList();
    Task<Gebruiker> getGebruiker(int id);
    Task<Gebruiker> getGebruiker(string name);
    Task<Gebruiker> UpdateGebruiker(Gebruiker gebruiker);
    Task<bool> DeleteGebruiker(int id);
}