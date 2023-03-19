using Models;

public class DeviceService : IDeviceService{
    private readonly IDbService _dbService;

    public DeviceService(IDbService dbservice){
        _dbService = dbservice;
    }

    public async Task<List<esp32>> getDeviceList(){
        return await _dbService.GetAll<esp32>("select * from public.devices", new{});
    }
    public async Task<esp32> getDevice(string ip, int port){
        return await _dbService.getSingle<esp32>("select * from public.devices where ip=@i and port=@p", new{i=ip, p=port});
    }
}