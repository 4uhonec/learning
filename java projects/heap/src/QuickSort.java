public class QuickSort {

    public void qsort(int[] arr){
        qsort(arr, 0, arr.length - 1);
    }

    private void qsort(int[] arr, int l, int r){
        if(l < r){
            int pivot = partition(arr, l, r);
            qsort(arr, l, pivot - 1);
            qsort(arr, pivot + 1, r);
        }
    }

    private int partition(int[] arr, int l, int r){
        int num = arr[r];
        int i = l-1;
        for(int j = l; j < r; j++){
            if(arr[j] <= num){
                i++;
                swap(arr, i, j);
            }
        }
        swap(arr, i + 1, r);
        return i + 1;
    }

    private void swap(int[] arr, int a, int b)
    {
        int temp = arr[a];
        arr[a] = arr[b];
        arr[b] = temp;
    }

}
