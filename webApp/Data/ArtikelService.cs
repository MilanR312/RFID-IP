using Models;
public class ArtikelService: IArtikelService{
    private readonly IDbService _dbService;

    public ArtikelService(IDbService dbService){
        _dbService = dbService;
    }

    public async Task<bool> CreateArtikel(Artikel artikel){
        var result = await _dbService.Insert<int>("INSERT INTO public.artikel (code, naam, beschikbaar, prijs, soort) VALUES(@code, @naam, @beschikbaar, @prijs, @soort);", artikel);
        return true;
    }
    public async Task<List<Artikel>> getArtikelList(){
        return await _dbService.GetAll<Artikel>("SELECT * from public.artikel;", new{});
    }
    public async Task<List<Artikel>> getArtikel(ArtikelRequest artikelreq){
        if (artikelreq.key != "")
            return await _dbService.GetAll<Artikel>("select * from public.artikel where code=@code", new{code=artikelreq.key});

        string model = "select * from public.artikel where";
        string query = model;
        if (artikelreq.naam != "")
            query += $" naam ilike '%{artikelreq.naam}%' and";
        if (artikelreq.operatorType != "")
            query += $" beschikbaar {artikelreq.operatorType} {artikelreq.beschikbaar} and";
        if (artikelreq.operatorType2 != "")
            query += $" prijs {artikelreq.operatorType2} {artikelreq.price} and";
        if (artikelreq.soort != "")
            query += $" soort ilike '%{artikelreq.soort}%' and";
        Console.WriteLine(query);
        if (model == query){
            return await getArtikelList();
        }
        return await _dbService.GetAll<Artikel>(query.Substring(0,query.Length-4), new{});
    }
    public async Task<Artikel> UpdateArtikel(Artikel artikel){
        var updateArtikel = await _dbService.Update<int>("UPDATE public.artikel SET code=@code, naam=@naam, beschikbaar=@beschikbaar, prijs=@prijs, soort=@soort;", artikel);
        return artikel;
    }
    public async Task<bool> DeleteArtikel(string key){
        await _dbService.Delete<int>("DELETE FROM public.artikel WHERE code=@code", new{code=key});
        return true;
    }

}