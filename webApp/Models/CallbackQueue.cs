
public class callbackQueue<T>{
    Mutex m = new();
    private Queue<(T, Action<string>, CancellationTokenSource)> items = new();

    CancellationTokenSource cs = new();

    public async Task<(T, Action<string>, CancellationTokenSource)> readBlocking(){
        Console.WriteLine("starting read process");
        try{
            if (items.Count == 0)
                await Task.Delay(Timeout.Infinite, cs.Token);
        } catch (OperationCanceledException){
        }
        Console.WriteLine("finished wait" + items.Count);
        m.WaitOne();
        var item = items.Dequeue();
        m.ReleaseMutex();
        Console.WriteLine("got data");
        //allow the add to complete

        Console.WriteLine("disposed temp");
        cs.Dispose();
        cs = new();
        Console.WriteLine("returning");
        return (item.Item1, item.Item2, item.Item3);
    }

    public async Task<bool> add(T item, Action<string> callback){
        CancellationTokenSource x = new();
        m.WaitOne();
        items.Enqueue((item, callback, x));
        m.ReleaseMutex();
        if(cs.Token.CanBeCanceled)
            cs.Cancel();
        Console.WriteLine("canceled main task");
        try{
            await Task.Delay(TimeSpan.FromMinutes(1), x.Token);
        } catch (OperationCanceledException){
            Console.WriteLine("adding task canceled");
            return false;
        }
        Console.WriteLine("         disconnected");
        //await Task.Delay(Timeout.Infinite, x.Token).ConfigureAwait(false);
        return true;
    }
}