using Models;

public interface IArtikelService{
    Task<bool> CreateArtikel(Artikel artikel);
    Task<List<Artikel>> getArtikelList();
    Task<List<Artikel>> getArtikel(ArtikelRequest areq);
    Task<Artikel> UpdateArtikel(Artikel artikel);
    Task<bool> DeleteArtikel(string code);
}