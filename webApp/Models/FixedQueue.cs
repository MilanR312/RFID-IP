using System.Collections;

public class FixedSizedQueue<T> : IEnumerable<T>
 {
     readonly Queue<T> q = new Queue<T>();

     public int Limit { get; set; }

     public FixedSizedQueue(int size){
        Limit = size;
     }
     public void Enqueue(T obj)
     {
        q.Enqueue(obj);
        if (q.Count > Limit)
            q.Dequeue();
     }

    public IEnumerator<T> GetEnumerator()
    {
        return q.GetEnumerator();
    }

    IEnumerator IEnumerable.GetEnumerator()
    {
        throw new NotImplementedException();
    }
}