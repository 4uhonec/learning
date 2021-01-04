public class Main {
    public static void main(String[] args) {
        int[] A = {12, 3, 5 ,50, 21, 24, 11, 5, 17, 8, 1, 1};
        bubble(A);
        for (int j : A) {
            System.out.print(j + " ");
        }
    }

    public static void bubble(int[] A)
    {
        int n = A.length;
        for(int i = 0; i < n - 1; i++)
        {
            for(int j = 0; j < n - i - 1; j++)
            {
                if(A[j]>A[j+1])
                {
                    int temp = A[j];
                    A[j] = A[j+1];
                    A[j+1] = temp;
                }
            }
        }
    }
}
