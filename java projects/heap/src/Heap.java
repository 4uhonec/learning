public class Heap {

    private int _size;
    private int[] _arr;

    public Heap(int[] arr)
    {
        _arr = arr;
        _size = _arr.length - 1;
    }

    private void swap(int a, int b)
    {
        int temp = _arr[a];
        _arr[a] = _arr[b];
        _arr[b] = temp;
    }

    //if new element inserted into heap in place i, makes order in this heap part
    private void heapify(int i)
    {
        int left = 2*i + 1;
        int right = 2*i + 2;
        int max = i;

        if(left <= _size && _arr[left] > _arr[max]) max = left;
        if(right <= _size && _arr[right] > _arr[max]) max = right;
        if(max != i )
        {
            swap(i,max);
            heapify(max);
        }
    }

    public void printHeap()
    {
        int temp = 1;
        for(int i = 0; i <= _size; i++)
        {
            System.out.print(_arr[i] + " ");
            if(i+2 == Math.pow(2, temp))
            {
                System.out.print("\n");
                temp++;
            }
        }
    }

    public int heapMax()
    {
        return _arr[0];
    }

    public int extractMax ()//READY
    {
        int max;
        if(_size < 1)
        {
            System.out.println("heap underflow");
            return -1;
        }
        max = _arr[0];
        _arr[0] = _arr[_size - 1];
        _size--;
        heapify(0);
        return max;
    }

    public void insertHeap(int num, int i)
    {
        if(i>_size)
        {
            System.out.println("Out of heap size");
            return;
        }
        _arr[i] = num;
        while(i>0 && _arr[i] > _arr[i/2])
        {
            swap(i, i/2);
            i = i/2;
        }
        heapify(i);
    }

    public void heapSort()
    {
        int tempSize = _size;
        for(int j = _size; j >= 1; j--){
            swap(0,j);
            _size--;
            heapify(0);
        }
        _size = tempSize;
    }

    public void buildHeap()
    {
        for(int i = _size/2; i>=0; i--){
            heapify(i);
        }
    }

    public void insertLeaf(int leaf)
    {

    }
}
