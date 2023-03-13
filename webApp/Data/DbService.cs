using Dapper;
using Npgsql;

public class DbService : IDbService{
    private readonly NpgsqlConnection _db; //posible datatype mismatch

    public DbService(IConfiguration config){
        _db = new NpgsqlConnection(config.GetConnectionString("RFID"));
        _db.Open();
    }

    public async Task<T> GetAsync<T>(string command, object parms){
        return (await _db.QueryAsync<T>(command, parms).ConfigureAwait(false)).FirstOrDefault();
    }

    public async Task<List<T>> GetAll<T>(string command, object parms){
        return (await _db.QueryAsync<T>(command, parms)).ToList();
    }
    public async Task<T> getSingle<T>(string command, object parms){
      return (await _db.QueryAsync<T>(command,parms)).FirstOrDefault();
    }
    public async Task<T> Insert<T>(string command, object parms)
  {
    return  ( await _db.QueryAsync<T>(command, parms)).FirstOrDefault();
  }
 
  public async Task<T> Update<T>(string command, object parms)
  {
    return (await _db.QueryAsync<T>(command, parms)).FirstOrDefault();
  }

  public async Task<T> Delete<T>(string command, object parms)
  {
    return (await _db.QueryAsync<T>(command, parms)).FirstOrDefault();
  }
}