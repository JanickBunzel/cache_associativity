import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Random;

public class MatrixMultiplication {
    public static void main(String[] args) {
        int n = 50; // Dimension der Matrizen
        int[][] A = generateMatrix(n);
        int[][] B = generateMatrix(n);
        int[][] C = new int[n][n];

        try (BufferedWriter writer = new BufferedWriter(new FileWriter("examples/algorithms/matrixMultiplicationJava_n.csv"))) {
            for (int i = 0; i < n; i++) {
                for (int j = 0; j < n; j++) {
                    C[i][j] = 0;
                    writer.write(String.format("W,%d,%d\n", getAddress(i, j, n), 0)); // Schreibzugriff initialisieren
                    System.out.println(String.format("W,%d,%d", getAddress(i, j, n), 0));
                    for (int k = 0; k < n; k++) {
                        writer.write(String.format("R,%d\n", getAddress(i, k, n))); // Lesezugriff auf A ohne Wert
                        writer.write(String.format("R,%d\n", getAddress(k, j, n))); // Lesezugriff auf B ohne Wert
                        C[i][j] += A[i][k] * B[k][j];
                        writer.write(String.format("W,%d,%d\n", getAddress(i, j, n), C[i][j])); // Schreibzugriff auf C
                    }
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static int[][] generateMatrix(int size) {
        Random random = new Random();
        int[][] matrix = new int[size][size];
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                matrix[i][j] = random.nextInt(100);
            }
        }
        return matrix;
    }

    private static int getAddress(int row, int col, int n) {
        return row * n + col;
    }
}