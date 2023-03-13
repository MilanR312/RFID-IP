using Models;

public class GebruikerService : IGebruikerService{
    private readonly IDbService _dbService;

    public GebruikerService(IDbService dbService){
        _dbService = dbService;
    }

    public async Task<bool> CreateGebruiker(Gebruiker gebruiker){
        var result = await _dbService.Insert<int>("INSERT INTO public.gebruiker (id, naam, leeftijd, adres, telefoonnummer, functie) VALUES (@id, @naam, @leeftijd, @adres, @telefoonNummer, @functie)", gebruiker);
        return true;
    }
    public async Task<List<Gebruiker>> getGebruikerList(){
        return await _dbService.GetAll<Gebruiker>("SELECT * FROM public.gebruiker", new {});
    }
    public async Task<Gebruiker> getGebruiker(int key){
        return await _dbService.getSingle<Gebruiker>("select * from public.gebruiker where id=@id", new{id=key});
    }
    public async Task<Gebruiker> getGebruiker(string key){
        return await _dbService.getSingle<Gebruiker>("select * from public.gebruiker where naam=@name", new{name=key});
    }
    public async Task<Gebruiker> UpdateGebruiker(Gebruiker gebruiker){
        var updateGebruiker = await _dbService.Update<int>("UPDATE scheme SET naam=@naam, leeftijd=@leeftijd, adres=@adres, telefoonnummer=@telefoonNummer, functie=@functie", gebruiker);
        return gebruiker;
    }
    public async Task<bool> DeleteGebruiker(int key){
        await _dbService.Delete<int>("DELETE FROM scheme where id=@id", new{id=key});
        return true;
    }
}