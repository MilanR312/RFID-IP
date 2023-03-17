using Models;

public interface IDeviceService{
    Task<List<esp32>> getDeviceList();
    Task<esp32> getDevice(string ip, int port);
}